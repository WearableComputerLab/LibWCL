PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

ifconfig > ip.txt

echo "put ip.txt" | sftp snappybackup@wcl.ml.unisa.edu.au

