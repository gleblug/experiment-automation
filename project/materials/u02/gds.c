#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <getopt.h>

// Actually I don't know if oscilloscope tty is freely accessible by conventional users
#define SETUID

#define GDS_BUFFER_LENGTH   1400026

char gds_buffer[GDS_BUFFER_LENGTH];
int gds_buffer_length;

#define ERR_DEVICE_OPEN    -1
#define ERR_DEVICE_READ    -2
#define ERR_INP_FILE_OPEN  -3
#define ERR_OUT_FILE_OPEN  -4
#define ERR_INV_CHANNEL    -5

struct record
{
  char *keyword;
  char *value;
};

// -------------------------------------------------------------------------
#define GDS_OPT_HELP    'h'
#define GDS_OPT_ASCII   'a'
#define GDS_OPT_BINARY  'b'
#define GDS_OPT_CONVERT 'c'
#define GDS_OPT_ACQUIRE 'g'
#define GDS_OPT_NO_HDR  'r'
#define GDS_OPT_INPUT   'i'
#define GDS_OPT_OUTPUT  'o'
#define GDS_OPT_DEVICE  'd'
#define GDS_OPT_CHANNEL 'n'

static const struct option gds_long_opt[] =
{
  {"help",    no_argument,       NULL, GDS_OPT_HELP},
  {"ascii",   no_argument,       NULL, GDS_OPT_ASCII},
  {"binary",  no_argument,       NULL, GDS_OPT_BINARY},
  {"convert", no_argument,       NULL, GDS_OPT_CONVERT},
  {"acquire", no_argument,       NULL, GDS_OPT_ACQUIRE},
  {"no-hdr",  no_argument,       NULL, GDS_OPT_NO_HDR},
  {"input",   required_argument, NULL, GDS_OPT_INPUT},
  {"output",  required_argument, NULL, GDS_OPT_OUTPUT},
  {"device",  required_argument, NULL, GDS_OPT_DEVICE},
  {"channel", required_argument, NULL, GDS_OPT_CHANNEL},
  {0,0,0,0}
};

static const char* gds_opt_str = "abcghri:o:d:n:";

// -------------------------------------------------------------------------
double gds_scan_double_key(char *str, struct record *keys, int max_keys)
{
  int curr_key;
  double result;
  for (curr_key = 0; curr_key < max_keys; curr_key++)
  {
    if (!strcmp(keys[curr_key].keyword, str))
    {
      sscanf(keys[curr_key].value, "%lf", &result);
      return result;
    }
  }
  return 0;
}

// -------------------------------------------------------------------------
int gds_scan_int_key(char *str, struct record *keys, int max_keys)
{
  int curr_key;
  int result;
  for (curr_key = 0; curr_key < max_keys; curr_key++)
  {
    if (!strcmp(keys[curr_key].keyword, str))
    {
      sscanf(keys[curr_key].value, "%d", &result);
      return result;
    }
  }
  return 0;
}

// -------------------------------------------------------------------------
int gds_acquire(char *dev_file_name, int channel_number)
{
  int dev_fd;
  int error;
#ifdef SETUID
  int old_uid, old_gid;
  int uid_changed = 0;
#endif // SETUID
  struct termios old_dev_tio, new_dev_tio;

  fd_set dev_read_fds;
  struct timeval dev_tv;

  char *dev_cmd_read_request = (channel_number == 1) ? "C1:WF? DAT2\n" : "C2:WF? DAT2\n";

  int dev_data_index = 0;
  int dev_read_len;

#ifdef SETUID
  if ((dev_fd = open(dev_file_name, O_RDWR | O_NOCTTY)) < 0) // check if we have permissions
  {
    old_uid = getuid(); old_gid = getgid();
    if (setuid(0) || setgid(0))
    {
      fprintf(stderr, "**Error**: You have no permissions to access \"%s\"\n"
                      "           Setting UID and GID to root failed\n", dev_file_name);
      return ERR_DEVICE_OPEN;
    }
    
    uid_changed = 1;
    // already root:wheel
    if ((dev_fd = open(dev_file_name, O_RDWR | O_NOCTTY)) < 0)
    {
      fprintf(stderr, "**Error**: Error opening device \"%s\"\n", dev_file_name);
      setuid(old_uid); setgid(old_gid);
      return ERR_DEVICE_OPEN;
    }
  }
#else // SETUID
  if ((dev_fd = open(dev_file_name, O_RDWR | O_NOCTTY)) < 0)
  {
    fprintf(stderr, "**Error**: Error opening device \"%s\"\n", dev_file_name);
    return ERR_DEVICE_OPEN;
  }
#endif // SETUID

  tcgetattr(dev_fd, &old_dev_tio);
  bzero(&new_dev_tio, sizeof(new_dev_tio));
  new_dev_tio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
  new_dev_tio.c_iflag = IGNBRK | IXON | IXOFF;
  new_dev_tio.c_oflag = 0;
  new_dev_tio.c_lflag = 0;
  new_dev_tio.c_cc[VTIME] = 5;
  new_dev_tio.c_cc[VMIN] = 1;
  new_dev_tio.c_ispeed = 13;
  new_dev_tio.c_ospeed = 13;
  tcflush(dev_fd, TCIFLUSH);
  tcsetattr(dev_fd, TCSANOW, &new_dev_tio);

  write(dev_fd, dev_cmd_read_request, strlen(dev_cmd_read_request));

  FD_ZERO(&dev_read_fds);
  FD_SET(dev_fd, &dev_read_fds);

  while (1)
  {
    dev_tv.tv_sec = 0;
    dev_tv.tv_usec = 200000;
    error = select(dev_fd + 1, &dev_read_fds, NULL, NULL, &dev_tv);
    switch(error)
    {
       case -1:
         fprintf(stderr, "**Error**: Error reading from device: select() failed with errorcode %d\n", error);
         close(dev_fd);
         return ERR_DEVICE_READ;
       case 0:
         close(dev_fd);
         gds_buffer_length = dev_data_index;
#ifdef SETUID
         if (uid_changed)  // reset uid/gid to create files belonging to current user but not root
         {
           setuid(old_uid);
           setgid(old_gid);
         }
#endif
         return 0;
       default:
         dev_read_len = read(dev_fd, gds_buffer + dev_data_index, GDS_BUFFER_LENGTH - dev_data_index);
         dev_data_index += dev_read_len;
    }
  }
  
}

// -------------------------------------------------------------------------
int gds_save_binary_file(char *filename)
{
  int out_fd;

  if ((out_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC)) < 0)
  {
    fprintf(stderr, "**Error**: Error opening output file %s\n", filename);
    return ERR_OUT_FILE_OPEN;
  } else
  {
    fchmod(out_fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ); // set 644 mode
    write(out_fd, gds_buffer, gds_buffer_length);
    close(out_fd);
    return 0;
  }
}

// -------------------------------------------------------------------------
int gds_read_binary_file(char *filename)
{
  int in_fd;

  if ((in_fd = open(filename, O_RDONLY)) == 0)
  {
    fprintf(stderr, "**Error**: Error opening data file \"%s\"\n", filename);
    return ERR_INP_FILE_OPEN;
  } else
  {
    gds_buffer_length = read(in_fd, gds_buffer, GDS_BUFFER_LENGTH);
    close(in_fd);
    return 0;
  }
}

// -------------------------------------------------------------------------
int gds_save_ascii_file(char *filename, int header_flag)
{
  #define MAX_KEYS  30
  struct record keys[MAX_KEYS];
  char *data_ptr;
  int i, header_length, curr_key, max_keys, max_key_len;
  double curr_time;
  FILE *out_file;

  union {
    short s;
    char b[2];
  } short_val;

  int Memory_Length;
  double Vertical_Scale;
  double Horizontal_Position;
  double Sampling_Period;
  
  if (( out_file = fopen(filename, "wt")) == NULL)
  {
    printf("**Error**: Error opening output file \"%s\"\n", filename);
    return ERR_OUT_FILE_OPEN;
  } else
  {
    for(i = 0; i < GDS_BUFFER_LENGTH; i++)
    {
      if (gds_buffer[i] == 10)
      {
        gds_buffer[i] = 0;
        header_length = i;
        break;
      }
    }
    
    for (curr_key = 0; curr_key < MAX_KEYS; curr_key++)
    {
      keys[curr_key].keyword = NULL;
      keys[curr_key].value = NULL;
    }
    
    i = 0;
    for(curr_key = 0; curr_key < MAX_KEYS; curr_key++)
    {
      keys[curr_key].keyword = gds_buffer + i;
      for( ; ; i++)
      {
        if (gds_buffer[i] == 0)
        {
          max_keys = curr_key;
          goto keys_done;
        }
        if (gds_buffer[i] == ',')
        {
          keys[curr_key].value = gds_buffer + i + 1;
          gds_buffer[i] = 0;
        }
        if (gds_buffer[i] == ';')
        {
          keys[curr_key + 1].keyword = gds_buffer + i + 1;
          gds_buffer[i] = 0;
          i++;
          break;
        }
      }
    }
keys_done:

    if (header_flag) // put header
    {
      max_key_len = 0;
      for (curr_key = 0; curr_key < max_keys; curr_key++)
      {
        if (strlen(keys[curr_key].keyword) > max_key_len)
        {
          max_key_len = strlen(keys[curr_key].keyword);
        }
      }
    
      for (curr_key = 0; curr_key < max_keys; curr_key++)
      {
        fprintf(out_file, "# \"%s\" ", keys[curr_key].keyword);
        for (i = 0; i < max_keys - strlen(keys[curr_key].keyword); i++)
        {
          fprintf(out_file, " ");
        }
        fprintf(out_file, "\"%s\"\n", keys[curr_key].value ? keys[curr_key].value : "");
      }
      fprintf(out_file, "\n");
    }

    Memory_Length       = gds_scan_int_key   ("Memory Length",       keys, max_keys);
    Vertical_Scale      = gds_scan_double_key("Vertical Scale",      keys, max_keys);
    Horizontal_Position = gds_scan_double_key("Horizontal Position", keys, max_keys);
    Sampling_Period     = gds_scan_double_key("Sampling Period",     keys, max_keys);

    curr_time = -(double)Memory_Length * Sampling_Period / 2.0 + Horizontal_Position;
    
    data_ptr = gds_buffer + header_length + 10; // skip #72000000
    fprintf(out_file, "# Time, s\tReading, raw\tReading, V\n");
    for (i = 0; i < Memory_Length; i++)
    {
      short_val.b[0] = data_ptr[2*i+1];
      short_val.b[1] = data_ptr[2*i];
      fprintf(out_file, "%le\t%d\t\t%le\n", curr_time, short_val.s, short_val.s * Vertical_Scale * 10.0 / 256.0);
      curr_time += Sampling_Period;
    }
    
    fclose(out_file);
    return 0;
  }
}

// -------------------------------------------------------------------------
void usage(void)
{
#ifdef SETUID
  #define SETUID_STATE "SETUID"
#else // SETUID
  #define SETUID_STATE
#endif // SETUID
  printf("gds [-abcgh] [-i <filename>] [-o <filename>] [-d <devname>]\n"
  "GDS-72302 oscilloscope data retrieving program " SETUID_STATE "\n"
  "(C) S. Ambrozevich <s.ambrozevich@mail.ru>, LPI\n"
  "v. 0.3 compiled " __DATE__ " " __TIME__ "\n\n"
  "Options:\n"
  "  -c, --convert       - convert previously saved binary data to ASCII form\n"
  "  -g, --acquire       - gather data from oscilloscope (default)\n"
  "  -r, --no-hdr        - do not store header information in ASCII output\n"
  "  -a, --ascii         - generate output in ASCII form\n"
  "  -b, --binary        - generate output in binary form (raw data)\n"
  "  -i, --input=file    - input file to convert, default = stdin\n"
  "  -o, --output=file   - output file to store data, default = stdout\n"
  "  -d, --device=dev    - device tty to communicate with, default = /dev/ttyACM0\n"
  "  -n, --channel=n     - channel number <1|2> to acquire, default = 2\n"
  "  -h, --help          - prints this help\n\n"
  "NOTE: \"gds -cb\" will only copy input binary stream to output\n");
}

// -------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int errorcode;
  int ascii_flag = 1;
  int convert_flag = 0;
  int header_flag = 1;
  int input_file_flag = 0;
  int output_file_flag = 0;
  int device_file_flag = 0;
  int channel_number = 2;
  int opt, opt_ind;

  char *input_file;
  char *output_file;
  char *device_file;

  if (argc == 1)
  {
    usage();
    return 0;
  }

  while ((opt = getopt_long(argc, argv, gds_opt_str, gds_long_opt, NULL)) != -1)
  {
    switch(opt)
    {
      case GDS_OPT_ASCII: 
        ascii_flag = 1;
        break;

      case GDS_OPT_BINARY:
        ascii_flag = 0;
        break;

      case GDS_OPT_CONVERT:
        convert_flag = 1;
        break;

      case GDS_OPT_ACQUIRE: 
        convert_flag = 0;
        break;

      case GDS_OPT_NO_HDR:
        header_flag = 0;
        break;

      case GDS_OPT_DEVICE:
        device_file_flag = 1;
        device_file = optarg;
        break;

      case GDS_OPT_INPUT: 
        input_file_flag = 1;
        input_file = optarg;
        break;

      case GDS_OPT_OUTPUT:
        output_file_flag = 1;
        output_file = optarg;
        break;

      case GDS_OPT_CHANNEL:
        sscanf(optarg, "%d", &channel_number);
        if ((channel_number < 1) || (channel_number > 2))
        {
          fprintf(stderr, "**Error**: Invalid channel number %d\n", channel_number);
          return ERR_INV_CHANNEL;
        }
        break;

      default:
        usage();
        return 0;
    }
  }

  argc -= optind;
  argv += optind;

  if (convert_flag)
  { // convert
    if (errorcode = gds_read_binary_file(input_file_flag ? input_file : "/dev/stdin") != 0)
    {
      return errorcode;
    }
  } else
  { // read
    if (errorcode = gds_acquire(device_file_flag ? device_file : "/dev/ttyACM0", channel_number) != 0)
    {
      return errorcode;
    }
  }

  if (ascii_flag)
  { // write ascii
    if (errorcode = gds_save_ascii_file(output_file_flag ? output_file : "/dev/stdout", header_flag) != 0)
    {
      return errorcode;
    }
  } else
  { // write binary
    if (errorcode = gds_save_binary_file(output_file_flag ? output_file : "/dev/stdout") != 0)
    {
      return errorcode;
    }
  }
 
  return 0;
}

