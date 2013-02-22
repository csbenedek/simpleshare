#!/bin/sh

defaults delete net.box.simpleshare
defaults delete net.box.simpleshare.BoxSimpleShare
sudo rm -Rf /Applications/Box\ SimpleShare.app
sudo rm -Rf /Applications/BoxSimpleShare.app
sudo rm -Rf /private/var/db/receipts/net.box.simpleshare.bom
sudo rm -Rf /private/var/db/receipts/net.box.simpleshare.plist
sudo rm -Rf /private/var/db/receipts/net.box.simpleshare.BoxSimpleShare.bom
sudo rm -Rf /private/var/db/receipts/net.box.simpleshare.BoxSimpleShare.plist
