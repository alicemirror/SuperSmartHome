# SuperSmartHome
Sources and other stuff for the SuperSmartHome project from the Connected Cloud Challenge by Element14

![SuperSmartHome logo and sponsors](https://github.com/alicemirror/SuperSmartHome/blob/master/Images/SuperSmartHome-Logo-Sponsors.jpg)

Here you find all the sources and materials (STL, circuit schematics, links) related to the projects part of the Super Smart Home challenge. Follow the whole challenge articles on [Element14.com](https://www.element14.com/community/community/design-challenges/connected-cloud-challenge-with-cypress-and-aws-iot/blog/authors/balearicdynamics?ICID=connectedcloud-challenger-doc)

A thankful note of appreciation to the challenge sponsors Element14, Cypress and AWS IoT Cloud together with [Elegoo](http://Elegoo.com) who provided all the 3D printing materials, as well as the 3D printers too and [Digitspace](https://www.digitspace.com/lora-433mhz-esp32-oled-0-96inch-ble-wireless-module?c8a5a9c712222e2c ), one of my preferred Open Source hardware seller for projects, sensors and microcontrollers.

## Important: the following article is used on some Python scripts used as a service in the projects of this repository

# How to Run a Script as a Service in Raspberry Pi - Raspbian Jessie
**By: Diego Acuña**

_Original Article: http://www.diegoacuna.me/how-to-run-a-script-as-a-service-in-raspberry-pi-raspbian-jessie/_

A pretty common task when using this device, is to run some script (for example a python script) as a 
service in the operating system so it can start on boot, stop and restart using systemctl and more. In 
this post I'm going to explain how to set a little script as a service using Raspbian Jessie in a Raspberry Pi.

## Example Python Script

First of all, we are going to write a small python script which print "Hello World" every 60 seconds. 
This is going to be our service script (hello_world.py):

```python
#!/usr/bin/python

from time import sleep

try:
    while True:
        print "Hello World"
        sleep(60)
except KeyboardInterrupt, e:
    logging.info("Stopping...")
```

You can execute it by python hello_world.py. If you get boring reading so many hello worlds, press Ctrl+C (or Cmd+C on OSX) to stop it. Save this file as hello_world.py in your home folder (home/pi/).

## Service for the Script

Now we're going to define the service to run this script:

```Shell
cd /lib/systemd/system/
sudo nano hello.service
```

The service definition must be on the /lib/systemd/system folder. Our service is going to be called "hello.service":

```text
[Unit]
Description=Hello World
After=multi-user.target

[Service]
Type=simple
ExecStart=/usr/bin/python /home/pi/hello_world.py
Restart=on-abort

[Install]
WantedBy=multi-user.target
```

Here we are creating a very simple service that runs our hello_world script and if by any means is aborted is going to be restarted automatically. You can check more on service's options in the next wiki: https://wiki.archlinux.org/index.php/systemd.

Now that we have our service we need to activate it:

```Shell
sudo chmod 644 /lib/systemd/system/hello.service
chmod +x /home/pi/hello_world.py
sudo systemctl daemon-reload
sudo systemctl enable hello.service
sudo systemctl start hello.service
```

## Service Tasks
For every change that we do on the /lib/systemd/system folder we need to execute a daemon-reload (third line of previous code). If we want to check the status of our service, you can execute:

`sudo systemctl status hello.service`

In general:

### Check status
`sudo systemctl status hello.service`

### Start service
`sudo systemctl start hello.service`

### Stop service
`sudo systemctl stop hello.service`

### Check service's log
`sudo journalctl -f -u hello.service`

## REFERENCES
- https://wiki.archlinux.org/index.php/systemd
- https://www.digitalocean.com/community/tutorials/understanding-systemd-units-and-unit-files
- https://coreos.com/os/docs/latest/getting-started-with-systemd.html
