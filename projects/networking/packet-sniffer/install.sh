#!/bin/bash
set -e

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
AmbientCapabilities=CAP_NET_RAW CAP_NET_ADMIN
CapabilityBoundingSet=CAP_NET_RAW CAP_NET_ADMIN

[Install]
WantedBy=multi-user.target
EOF

echo "Enabling and starting service"
systemctl daemon-reload
systemctl enable --now pcapsniffer.service

echo "Done"
echo "Use /var/lib/pcap-sniffer/captures.pcap"
