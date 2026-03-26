#!/bin/bash

if [[ $EUID -ne 0 ]]; then
    echo "Root required"
    exit 1
fi

echo "Creating service user"
useradd --system --shell /bin/nologin pcapsnifferd
passwd -l pcapsnifferd

echo "Compiling and installing"
gcc main.c -o pcapsnifferd
install -m 755 -o pcapsnifferd -g pcapsnifferd pcapsnifferd /usr/local/bin/pcapsnifferd
rm -f pcapsnifferd

echo "Creating systemd service"
cat > /etc/systemd/system/pcapsniffer.service << EOF
[Unit]
Description=Passive packet sniffer utility
After=network.target

[Service]
Type=simple
ExecStart=/usr/local/bin/pcapsnifferd
Restart=on-failure
User=pcapsnifferd
StateDirectory=pcap-sniffer
WorkingDirectory=/var/lib/pcap-sniffer
AmbientCapabilities=CAP_NET_RAW CAP_NET_ADMIN
CapabilityBoundingSet=CAP_NET_RAW CAP_NET_ADMIN

[Install]
WantedBy=multi-user.target
EOF

echo "Enabling and starting service"
systemctl daemon-reload
systemctl enable --now pcapsniffer.service

echo -e "Done\n"
echo "Copy capture from remote machine: scp user@host:/var/lib/pcap-sniffer/captures.pcap ./captures.pcap"
echo "View: tcpdump -r captures.pcap  or  wireshark captures.pcap"
