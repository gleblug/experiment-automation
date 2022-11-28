#include "Oscilloscope.hpp"

Oscilloscope::Oscilloscope(std::string server_ip, port_t server_port) : m_channel("C1")
{
    // setup a socket and connection tools
    struct hostent *host = gethostbyname(server_ip.c_str());

    sockaddr_in sendSockAddr;
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr =
        inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    sendSockAddr.sin_port = htons(server_port);

    m_client_side = socket(AF_INET, SOCK_STREAM, 0);

    // try to connect...
    int status = connect(m_client_side,
                         (sockaddr *)&sendSockAddr, sizeof(sendSockAddr));

    if (status < 0)
        throw std::runtime_error("Error connecting to oscilloscope!");

    std::cout << "Connected to the oscilloscope!" << std::endl;

    char *msg = new char[256];
    recv(m_client_side, msg, 256, 0);
    std::cout << msg << std::endl;
}

std::string Oscilloscope::request()
{
    size_t msg_size = 256;
    char buf[msg_size];
    recv(m_client_side, buf, msg_size, 0);

    return std::string(buf, strlen(buf));
}

size_t Oscilloscope::request(char *buf, size_t msg_size)
{
    size_t bytes_read = 0;
    while (bytes_read < msg_size)
        bytes_read += recv(m_client_side, std::next(buf, bytes_read), msg_size - bytes_read, 0);

    return bytes_read;
}

std::string Oscilloscope::query(std::string const &comm)
{
    command(comm);
    usleep(50000);
    return request();
}

double Oscilloscope::parser(std::string const &comm, std::string const &to_find, std::string const &units)
{
    std::string response = query(comm);

    ind_t value_ind = response.find(to_find) + to_find.length() + 1;
    response.erase(0, value_ind - 1);

    ind_t units_ind = response.find(units);
    response.erase(units_ind);

    return std::stod(response);
}

void Oscilloscope::save_waveform(std::string file_name)
{
    std::size_t const header_size = 23;
    char *header = new char[header_size];

    command(m_channel + ":WF? DAT2");
    request(header, header_size);

    // calculate bytes count to read
    size_t pos = 14; // C1:WF DAT2,#9002800000
    size_t number = header[13] - '0';

    std::string bytes_count_str = std::string(header, header_size).substr(pos, number);
    size_t bytes_count = std::stoi(bytes_count_str);

    // request waveform
    char *wf = new char[bytes_count];
    request(wf, bytes_count);

    // end values
    usleep(100000);
    auto endv = request();

    if ((endv[0] != 10) || (endv[1] != 10) || (endv.length() != 2))
        std::cerr << "Invalid end values: " << static_cast<int>(endv[0]) << ' ' << static_cast<int>(endv[1]) << std::endl;

    // parse waveform
    std::ofstream out;
    out.open(file_name, std::ios_base::trunc | std::ios_base::out);

    out << "voltage,time\n";

    double vdiv = get_vdiv() / 25.0f;
    double voffset = get_voffset();

    double timebase = get_timebase();
    double sampling_rate = get_sampling_rate();

    double time_inter = 1.0f / sampling_rate;
    double const grid = 14; // The grid numbers in horizontal direction

    double time_value = -(timebase * grid / 2.0f);

    std::cout
        << "Vdiv:           " << vdiv << std::endl
        << "Voffset:        " << voffset << std::endl
        << "Timebase:       " << timebase << std::endl
        << "Sampling rate:  " << sampling_rate << std::endl;

    for (size_t i = 0; i < bytes_count; i++)
        out << wf[i] * vdiv - voffset << ',' << time_value + time_inter * i << '\n';

    out.close();
}

Oscilloscope::~Oscilloscope()
{
    close(m_client_side);
    std::cout << "Connection closed" << std::endl;
}
