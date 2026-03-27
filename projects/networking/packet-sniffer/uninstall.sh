#!/bin/bash

if [[ $EUID -ne 0 ]]; then
    echo "Root required"
    exit 1
fi

echo "Stopping and disabling service"
systemctl stop pcapsniffer
systemctl disable pcapsniffer

echo "Removing unit file"
rm -rf /etc/systemd/system/pcapsniffer.service
systemctl daemon-reload

echo "Removing binary"
rm /usr/local/bin/pcapsnifferd

echo "Removing service user"
userdel pcapsnifferd

read -r -p "Delete capture directory and file? [y/N]: " ans

case "$ans" in
    y|Y|Yes|YES|yes)
        rm -rf /var/lib/pcap-sniffer/
        ;;
    *)
        echo "Not deleting"
        ;;
esac

echo -e "\nDone"
