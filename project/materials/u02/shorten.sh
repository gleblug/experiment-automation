#!/bin/sh
#works only for ascii files with headers
#prints every 100th line
cat $1 | sed '1,/Waveform/d' | awk '!(NR%100)||(NR==2)'> $2