# cc

***crap compilation of mostly Linux and C/C++ stuff***


**Disclaimer : I am not liable for any misuse of this code Only use it on machines and in environments that you have explicit permissions and authrizations to do so**


# LINUX

```shell

# kernel mode

process & memory (arch_mem)
   process, hardware interrupt -> kernel -> process -> hardware interrupt, threads
   memory, kernel space, user space, virtual address, physical address, page table, mmu, major fault, minor fault
   ps, top, lsof, strace, ltrace, renice, uptime, trace-cmd, kernelshark
   vmstat, iostat, iotop, pidstat
   cgroup, /proc/self/cgroup, /sys/fs/cgroup

# device (driver)
   rootfs, /bin, /dev, /etc, /home, /lib, /proc, /run, /sys, /sbin, /tmp, /usr, /var, /boot, /media, /opt
   /dev, block, character, pipe, socket, user i/o oriented, /sys/devices, actual device info
     udev, devtmpfs, kernel sends uevent to udev, udev initializes and registers devices, in devtmpfs when booting
     udevadm info --query=all --name=/dev/sda, udevadm monitor
     kernel block device interface < - > scsi device driver < - > bus < - > (ATA, USB...) bridge driver < - > device
   disk, partition table -> partition -> fs data structure -> fs (maybe rootfs) data
     parted -l, fdisk /dev/sdd, mkfs -t ext4 /dev/sdf2, mount -t ext4 /dev/sdf2 /home/some, umount /home/some, blkid, /etc/fstab
     lvm, pvs, pvdisaply, vgs, vgdisplay, lvs, lvdisplay, pvcreate,vgcreate, vgextend, lvcreate, lvextend
     block device (kernel) > devmapper (kernel) > lvm (user)
     inode (data pointer), block bitmap (allocation pointer), fsck > inode != block bitmap > lost + found
   network, physical, internet, transport, application, ipv4, ipv6
     ping, host, ip, iw, linux network interface
     netplan, /etc/netplan, networkmanager, /etc/NetworkManager
     /etc/hosts, /etc/resolv.conf, /etc/nsswitch.conf
     netstat, tcp, udp
     iptables
     user process < - > kernel socket (system call) < - > network
     ipc (inter process communication) > unix domain socket > eg) mysql.sock
     rsync
# syscall & user mode (os)
```

```shell

harware (interrupt)

kernel vas

  interrupt (hard, soft irq)

  process 

process vas

  user (syscall)


```


```shell

# process & memory

# procfs : /proc/*

# kernel stacks

cat /proc/$PID/stack

# user stacks

gdb

# both

# use eBPF

sudo stackcount-bpfcc -p $PID -v -d

# device

# sysfs: /sys/*

```

```shell
# cgroup 

# check cgroup mount location

mount | grep cgroup2

# if not change kernel parameters in grub to see

# cgroup_no_v1=all

cat /proc/cmdline

# check controllers

cat /sys/fs/cgroup/cgroup.controllers

# add controller , here SOMETHING being cpu 

echo "+$SOMETHING" >> /sys/fs/cgroup/cgroup.subtree_control  

# add sub group

mkdir /sys/fs/cgroup/$SOME_GROUP

# give cpu max

echo "$MAX $PERIOD" > /sys/fs/cgroup/$SOME_GROUP/cpu.max

# revoke group

rmdir /sys/fs/cgroup/$SOME_GROUP


```

# LINUX KERNEL MODULE

```shell

lsmod

insmod
rmmod

depmod

modprobe

modprobe -r



```
```shell
# auto load:  insert module load command to

# /etc/modules-load.d/foo.conf
# or
# /etc/modprobe.d/foo.conf
foo

```

```shell

# blacklist: insert line to

# /etc/modules-load.d/foo.conf
# or
# /etc/modprobe.d/foo.conf
blacklist foo

```

```shell

# dkms
# autobuild when kernel update

# add dkms.conf
dkms add . -m $MODULE_NAME -v $VERSION

dkms autoinstall

# or

dkms build $MODULE -v $VERSION

dkms install $MODULE -v $VERSION


```

# LINUX BOOT

```shell
boot (firm)

bios | uefi -> grub -> vmlinuz, initramfs -> device and drivers -> rootfs mount -> init (pid 1, systemd, user space) -> log in

kernel inspection, cpu, memory, device bus, device, aux kernel subsys(networking...), rootfs, user space

init(systemd)
  systemctl, unit, /etc/systemd, /lib/systemd, /usr/systemd...
    target, service, socket, mount...
```

# GRUB

```shell
# check partition and images

ls

linux (hd0,1)/vmlinuz root=/dev/sda1

initrd (hd0,1)/initrd.img

boot

```

# LINUX KERNEL BUILD

```shell

# packages

sudo apt update


sudo apt install -y git fakeroot build-essential tar ncurses-dev \
    tar xz-utils libssl-dev bc stress python3-distutils libelf-dev \
    linux-headers-$(uname -r) bison flex libncurses5-dev util-linux net-tools "linux-tools-$(uname -r)" exuberant-ctags cscope \
    sysfsutils gnome-system-monitor curl perf-tools-unstable \
    gnuplot rt-tests indent tree psmisc smem libnuma-dev numactl \
    hwloc bpfcc-tools sparse flawfinder cppcheck bsdmainutils \
    trace-cmd virt-what dwarves 


# get source

curl -o /tmp/linux-5.5.1.tar.xz https://mirrors.edge.kernel.org/pub/linux/kernel/v5.x/linux-5.5.1.tar.xz

# or clone

git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git

mv /tmp/linux-5.5.1.tar.xz ~

cd ~

tar -xf linux-5.5.1.tar.xz

# default
    LLKD_KSRC="$HOME/linux-5.5.1"
    cp /boot/config-5.4.0-148-generic "$LLKD_KSRC/.config"
    cd "$LLKD_KSRC"
    make menuconfig

# localmod
    LLKD_KSRC="$HOME/linux-5.5.1"
    lsmod > /tmp/lsmod.now
    cd "$LLKD_KSRC"
    make LSMOD=/tmp/lsmod.now localmodconfig
    make menuconfig

# ubuntu specific

scripts/config --disable SYSTEM_TRUSTED_KEYS
scripts/config --disable SYSTEM_REVOCATION_KEYS

# change config

LLKD_KSRC="$HOME/linux-5.5.1"

cp "$LLKD_KSRC/init/Kconfig" "$LLKD_KSRC/init/Kconfig.orig"

vim "$LLKD_KSRC/init/Kconfig"

# build

cd "$LLKD_KSRC"

time make -j4


# module install

cd "$LLKD_KSRC"

sudo make modules_install

ls /lib/modules


# boot image and initramfs

LLKD_KSRC="$HOME/linux-5.5.1"


cd "$LLKD_KSRC"

sudo make install


# boot update 

sudo cp /etc/default/grub /etc/default/grub.orig

sudo vim /etc/default/grub

sudo update-grub


# ui mode switch

Ctrl + Alt + F2


# make iso

sudo apt install genisofs

mkdir /bck

# with isolinux at the top "/boot" directroy - relative to source path

mkisofs -b boot/isolinux/isolinux.bin -c boot/isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -J -joliet-long -R -x /bck -x /proc -x /tmp -x /mnt -x /dev -x /sys -x /run -x /media -x /var/log -x /var/cache/apt/archives -o /bck/<name>.iso /

# then extract iso using scp or whatever

sudo isohybrid /bck/<output>.iso


```

# LINUX DIAGNOSTIC

```shell
journalctl -f
journalctl -xe
journalctl -k
journalctl -b
dmesg
efibootmgr
```
# GDB DEBUGGER C C++ GO BINARY DEBUG

```shell
Startup 
% gdb -help         	print startup help, show switches
*% gdb object      	normal debug 
*% gdb object core 	core debug (must specify core file)
%% gdb object pid  	attach to running process
% gdb        		use file command to load object 

Help
*(gdb) help        	list command classes
(gdb) help running      list commands in one command class
(gdb) help run        	bottom-level help for a command "run" 
(gdb) help info         list info commands (running program state)
(gdb) help info line    help for a particular info command
(gdb) help show         list show commands (gdb state)
(gdb) help show commands        specific help for a show command

Breakpoints
*(gdb) break main       set a breakpoint on a function
*(gdb) break 101        set a breakpoint on a line number
*(gdb) break basic.c:101        set breakpoint at file and line (or function)
*(gdb) info breakpoints        show breakpoints
*(gdb) delete 1         delete a breakpoint by number
(gdb) delete        	delete all breakpoints (prompted)
(gdb) clear             delete breakpoints at current line
(gdb) clear function    delete breakpoints at function
(gdb) clear line        delete breakpoints at line
(gdb) disable 2         turn a breakpoint off, but don't remove it
(gdb) enable 2          turn disabled breakpoint back on
(gdb) tbreak function|line        set a temporary breakpoint
(gdb) commands break-no ... end   set gdb commands with breakpoint
(gdb) ignore break-no count       ignore bpt N-1 times before activation
(gdb) condition break-no expression         break only if condition is true
(gdb) condition 2 i == 20         example: break on breakpoint 2 if i equals 20
(gdb) watch expression        set software watchpoint on variable
(gdb) info watchpoints        show current watchpoints

Running the program
*(gdb) run        	run the program with current arguments
*(gdb) run args redirection        run with args and redirection
(gdb) set args args...        set arguments for run 
(gdb) show args        show current arguments to run
*(gdb) cont            continue the program
*(gdb) step            single step the program; step into functions
(gdb) step count       singlestep \fIcount\fR times
*(gdb) next            step but step over functions 
(gdb) next count       next \fIcount\fR times
*(gdb) CTRL-C          actually SIGINT, stop execution of current program 
*(gdb) attach process-id        attach to running program
*(gdb) detach        detach from running program
*(gdb) finish        finish current function's execution
(gdb) kill           kill current executing program 

Stack backtrace
*(gdb) bt        	print stack backtrace
(gdb) frame        	show current execution position
(gdb) up        	move up stack trace  (towards main)
(gdb) down        	move down stack trace (away from main)
*(gdb) info locals      print automatic variables in frame
(gdb) info args         print function parameters 

Browsing source
*(gdb) list 101        	list 10 lines around line 101
*(gdb) list 1,10        list lines 1 to 10
*(gdb) list main  	list lines around function 
*(gdb) list basic.c:main        list from another file basic.c
*(gdb) list -        	list previous 10 lines
(gdb) list *0x22e4      list source at address
(gdb) cd dir        	change current directory to \fIdir\fR
(gdb) pwd          	print working directory
(gdb) search regexpr    forward current for regular expression
(gdb) reverse-search regexpr        backward search for regular expression
(gdb) dir dirname       add directory to source path
(gdb) dir        	reset source path to nothing
(gdb) show directories        show source path

Browsing Data
*(gdb) print expression        print expression, added to value history
*(gdb) print/x expressionR        print in hex
(gdb) print array[i]@count        artificial array - print array range
(gdb) print $        	print last value
(gdb) print *$->next    print thru list
(gdb) print $1        	print value 1 from value history
(gdb) print ::gx        force scope to be global
(gdb) print 'basic.c'::gx        global scope in named file (>=4.6)
(gdb) print/x &main     print address of function
(gdb) x/countFormatSize address        low-level examine command
(gdb) x/x &gx        	print gx in hex
(gdb) x/4wx &main       print 4 longs at start of \fImain\fR in hex
(gdb) x/gf &gd1         print double
(gdb) help x        	show formats for x
*(gdb) info locals      print local automatics only
(gdb) info functions regexp         print function names
(gdb) info variables  regexp        print global variable names
*(gdb) ptype name        print type definition
(gdb) whatis expression       print type of expression
*(gdb) set variable = expression        assign value
(gdb) display expression        display expression result at stop
(gdb) undisplay        delete displays
(gdb) info display     show displays
(gdb) show values      print value history (>= gdb 4.0)
(gdb) info history     print value history (gdb 3.5)

Object File manipulation
(gdb) file object      		load new file for debug (sym+exec)
(gdb) file             		discard sym+exec file info
(gdb) symbol-file object        load only symbol table
(gdb) exec-file object 		specify object to run (not sym-file)
(gdb) core-file core   		post-mortem debugging

Signal Control
(gdb) info signals        	print signal setup
(gdb) handle signo actions      set debugger actions for signal
(gdb) handle INT print          print message when signal occurs
(gdb) handle INT noprint        don't print message
(gdb) handle INT stop        	stop program when signal occurs
(gdb) handle INT nostop         don't stop program
(gdb) handle INT pass        	allow program to receive signal
(gdb) handle INT nopass         debugger catches signal; program doesn't
(gdb) signal signo        	continue and send signal to program
(gdb) signal 0        		continue and send no signal to program

Machine-level Debug
(gdb) info registers        	print registers sans floats
(gdb) info all-registers        print all registers
(gdb) print/x $pc        	print one register
(gdb) stepi        		single step at machine level
(gdb) si        		single step at machine level
(gdb) nexti        		single step (over functions) at machine level
(gdb) ni        		single step (over functions) at machine level
(gdb) display/i $pc        	print current instruction in display
(gdb) x/x &gx        		print variable gx in hex
(gdb) info line 22        	print addresses for object code for line 22
(gdb) info line *0x2c4e         print line number of object code at address
(gdb) x/10i main        	disassemble first 10 instructions in \fImain\fR
(gdb) disassemble addr          dissassemble code for function around addr

History Display
(gdb) show commands        	print command history (>= gdb 4.0)
(gdb) info editing       	print command history (gdb 3.5)
(gdb) ESC-CTRL-J        	switch to vi edit mode from emacs edit mode
(gdb) set history expansion on       turn on c-shell like history
(gdb) break class::member       set breakpoint on class member. may get menu
(gdb) list class::member        list member in class
(gdb) ptype class               print class members
(gdb) print *this        	print contents of this pointer
(gdb) rbreak regexpr     	useful for breakpoint on overloaded member name

Miscellaneous
(gdb) define command ... end        define user command
*(gdb) RETURN        		repeat last command
*(gdb) shell command args       execute shell command 
*(gdb) source file        	load gdb commands from file
*(gdb) quit        		quit gdb

```


# ON WINDOWS 

```shell
virtual machine -> bridge  

 

wsl bridge -> netsh interface portproxy add v4tov4 listenaddress=0.0.0.0 listenport=<winport> connectaddress=localhost connectport=linuxport 

```
 
# ONEDRIVE

```shell

# installing onedrive cmd

sudo apt update 

sudo apt install onedrive -y


# custome configuration

wget https://raw.githubusercontent.com/abraunegg/onedrive/master/config -O ~/.config/onedrive/config


vim ~/.config/onedrive/config


```

```shell 

# ~/.config/onedrive/config

...

sync_dir = "~/c"
...

```

```shell

# ~/.config/onedrive/sync_list
...
/c
...


```

```shell


# no autostart

sudo rm /etc/systemd/user/default.target.wants/onedrive.service

# autostart
# symlink

/etc/systemd/user/default.target.wants/onedrive.service → /usr/lib/systemd/user/onedrive.service

```

```shell

# sync single directory

onedrive --synchronize --single-directory "c" 


``` 

# PROFILE PATH PERMANENT

```shell

vim .profile

source .profile

```
 
# GIT

```shell

# local branch

git switch -c <loc_name> <remote_name>/<branch>

# local merge
# on target branch

git merge <loc_name>

# on forked remote branch's local branch
# pulling equivalent

git remote add <upstream_name> <upstream_addr>

git fetch <upstream_name>

git rebase <upstream_name>/<branch>

git add .

git commit

git push -f <forked_origin> <forked_origin_branch>

# then PR

# squash commits

git log

git reset --soft HEAD~<last_commits_count>

ex) git reset --soft HEAD~5

# config 

git config --global credential.helper store

# then clone or pull or whatever to store

# private repo
# automatic with go

git config --global set url."https://$ID:$PW@github.com/org".instreadOf "https://github.com/org"

# github workflow self hosted runner 

# Create a folder
mkdir actions-runner && cd actions-runner

# Download the latest runner package

curl -o actions-runner-linux-x64-2.306.0.tar.gz -L https://github.com/actions/runner/releases/download/v2.306.0/actions-runner-linux-x64-2.306.0.tar.gz

# Optional: Validate the hash

echo "b0a090336f0d0a439dac7505475a1fb822f61bbb36420c7b3b3fe6b1bdc4dbaa  actions-runner-linux-x64-2.306.0.tar.gz" | shasum -a 256 -c

# Extract the installer

tar xzf ./actions-runner-linux-x64-2.306.0.tar.gz

# Create the runner and start the configuration experience

./config.sh --url https://github.com/seantywork/k8s-base-cluster-setup --token <TOKEN>

# Last step, run it!

./run.sh

```


```shell

# install github cli

curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg | sudo dd of=/usr/share/keyrings/githubcli-archive-keyring.gpg 

sudo chmod go+r /usr/share/keyrings/githubcli-archive-keyring.gpg

echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | sudo tee /etc/apt/sources.list.d/github-cli.list > /dev/null 

sudo apt update

sudo apt install gh -y

```

```shell

# some commands

gh pr list

gh pr edit <num> --add-reviewer seantywork

gh pr edit <num> --add-assignee seantywork

gh pr merge <num> --merge

gh pr view <num> --json state,author,assignees,reviews,reviewRequests,mergeable

```


# PURCHASE 

 
```shell
Server - AWS EC2 instance, Http, https, ssh inbound setting  

Domain Name - Namecheap - two A RECORD set for host @, www, and link IP  

openai ready 
```
 

# OS VERSION 

```shell



AWS EC2 Ubuntu 22 

``` 

# INSTALL 

 
```shell
chrome 

virtualbox 

git 

curl 

wget 

vim 

vscode 

python3-pip 

nodejs 

go 

docker 

kubectl 

virtualbox 

ghidra 

wireshark 

metasploit 

nmap 

sqlmap 

nikto 

burpesuite 

owasp zap 

tor 

 

nginx 

gunicorn 

mysql 

python 

pip 

django 

pandas 

openai 

mysql-connector-python 

mysqlclient 

```

# MYSQL SETTING 

 
```shell
# create user, grant privileges 

python manage.py migrate 
```
 

# GUNICORN

```shell


mkdir -pv config/gunicorn/ 



```

```python

# gunicorn dev.py 

"""Gunicorn *development* config file""" 

# Django WSGI application path in pattern MODULE_NAME:VARIABLE_NAME 

wsgi_app = "<project_name>.wsgi:application" 

# The granularity of Error log outputs 

loglevel = "debug" 

# The number of worker processes for handling requests 

workers = 2 

# The socket to bind 

bind = "0.0.0.0:8000" 

# Restart workers when code changes (development only!) 

reload = True 

# Write access and error info to /var/log 

accesslog = errorlog = "/var/log/gunicorn/dev.log" 

# Redirect stdout/stderr to log file 

capture_output = True 

# PID file so you can easily fetch process ID 

pidfile = "/var/run/gunicorn/dev.pid" 

# Daemonize the Gunicorn process (detach & enter background) 

daemon = True 


```

```shell
sudo mkdir -pv /var/{log,run}/gunicorn/ 

 

sudo chown -cR ubuntu:ubuntu /var/{log,run}/gunicorn/ 

 

gunicorn -c config/gunicorn/dev.py 
```

# NGINX SETTING


```shell
sudo mkdir -pv /var/www/<project_name>/static/ 

sudo chown -cR ubuntu:ubuntu /var/www/<project_name>/  

 

django setting.py debug false 

python manage.py collectstatic 
```

# HTTPS

```shell

# Nginx configuration: /etc/nginx/sites-available/<nginx_config_name> 

server_tokens               off; 

access_log                  /var/log/nginx/<nginx_config_name>.access.log; 

error_log                   /var/log/nginx/<nginx_config_name>.error.log; 

 

server { 

  server_name               .<domain_name>; 

  listen                    80; 

  return                    307 https://$host$request_uri; 

} 

 

server { 

  location / { 

    proxy_pass              http://localhost:8000; 

    proxy_set_header        Host $host; 

    proxy_set_header    X-Forwarded-Proto $scheme; 

  } 

 

  location /static { 

    autoindex on; 

    alias <static_location>; 

  } 

 

  listen 443 ssl; 

  ssl_certificate /etc/letsencrypt/live/www.supersecure.codes/fullchain.pem; 

  ssl_certificate_key /etc/letsencrypt/live/www.supersecure.codes/privkey.pem; 

  include /etc/letsencrypt/options-ssl-nginx.conf; 

  ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem; 

} 


```

```shell

sudo systemctl restart nginx 

 

cd /etc/nginx/sites-enabled 

sudo ln -s ../sites-available/<nginx_config_name> . 

 

sudo vi /etc/nginx/nginx.conf 

ssl_protocols TLSv1.2 TLSv1.3; 

include /etc/nginx/sites-enabled/<nginx_config_name> 

sudo systemctl restart nginx 

 

sudo snap install --classic certbot 

sudo ln -s /snap/bin/certbot /usr/bin/certbot 

sudo certbot --nginx --rsa-key-size 4096 --no-redirect 

 

sudo systemctl restart nginx 

 


```


# AWS RESTART

```shell
# ip and dns check 

# aws new ssh domain and ip check  

# namecheap DNS relinking 

```

# DB CHECK

```shell

# running

```

# GUINCORN CHECK

```shell
sudo mkdir -pv /var/{log,run}/gunicorn/ 

sudo chown -cR ubuntu:ubuntu /var/{log,run}/gunicorn/ 

gunicorn -c config/gunicorn/dev.py 


```



# NGINX CHECK 

```shell



 

stream { 

        upstream galera_cluster { 

            server 127.0.0.1:33061; #node1 

            server 127.0.0.1:33062; #node2 

            server 127.0.0.1:33063; #node3 

            zone tcp_mem 64k; 

            least_conn; 

        } 

 

        server { 

            listen 3306; # MySQL default 

            proxy_pass galera_cluster; 

        } 

} 

 

 

http  

 

        sendfile on; 

        tcp_nopush on; 

        tcp_nodelay on; 

        reset_timedout_connection on; 

        client_body_timeout 3600; 

        send_timeout 2; 

 

        keepalive_timeout 30; 

        keepalive_requests 100000; 

 

        #types_hash_max_size 2048; 

        # server_tokens off; 

 

        client_max_body_size 40G; 

        client_body_buffer_size 16K; 

        client_header_buffer_size 1K; 

 

        proxy_request_buffering off; 

 

        proxy_send_timeout 3600; 

 

server 

 

location 

 

                proxy_pass http://agc_server; 

                proxy_read_timeout 300s; 

                proxy_connect_timeout 75s; 

                proxy_set_header X-Real-IP $remote_addr; 

                #proxy_set_header Host $host; 

                proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for; 

                proxy_set_header X-Forwarded-For $remote_addr; 

                proxy_set_header X-Forwarded-Proto $scheme; 

                proxy_set_header Host $http_host; 

                proxy_http_version 1.1; 

                proxy_set_header   Upgrade $http_upgrade; 

                proxy_set_header   Connection "upgrade"; 

 

 

```


```shell
# or

upstream UC {

  server 0.0.0.0:8888;

}

upstream PTT {

  server 0.0.0.0:8888;

}


server
{
        listen       80;
        server_name      usagecorpus.com www.usagecorpus.com;
        return 301 https://$host$request_uri;
}

server
{
        listen       80;
        server_name      ptt.usagecorpus.com;
        return 301 https://$host$request_uri;
}



server
{
        server_name  usagecorpus.com www.usagecorpus.com;
        client_max_body_size 0;

        location / {
            # add_header Content-Type text/plain;
            # return 200 'okay';

             proxy_pass http://UC;
             proxy_set_header X-Real-IP $remote_addr;
             proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
             proxy_set_header X-Forwarded-For $remote_addr;
             proxy_set_header X-Forwarded-Proto $scheme;
             proxy_set_header Host $http_host;

       #      proxy_buffering off;
       #      proxy_request_buffering off;

       #      proxy_http_version 1.1;
       #      proxy_set_header   Upgrade $http_upgrade;
       #      proxy_set_header   Connection "upgrade";


      }


    #listen 80; # managed by Certbot

    listen 443 ssl; # managed by Certbot
    ssl_certificate /etc/letsencrypt/live/usagecorpus.com/fullchain.pem; # managed by Certbot
    ssl_certificate_key /etc/letsencrypt/live/usagecorpus.com/privkey.pem; # managed by Certbot
    include /etc/letsencrypt/options-ssl-nginx.conf; # managed by Certbot
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem; # managed by Certbot


}

server
{
        server_name  ptt.usagecorpus.com;
        client_max_body_size 0;

        location / {
            # add_header Content-Type text/plain;
            # return 200 'okay';

             proxy_pass http://PTT;
             proxy_set_header X-Real-IP $remote_addr;
             proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
             proxy_set_header X-Forwarded-For $remote_addr;
             proxy_set_header X-Forwarded-Proto $scheme;
             proxy_set_header Host $http_host;

       #      proxy_buffering off;
       #      proxy_request_buffering off;

       #      proxy_http_version 1.1;
       #      proxy_set_header   Upgrade $http_upgrade;
       #      proxy_set_header   Connection "upgrade";


       }


    #listen 80; # managed by Certbot

    listen 443 ssl; # managed by Certbot
    ssl_certificate /etc/letsencrypt/live/usagecorpus.com/fullchain.pem; # managed by Certbot
    ssl_certificate_key /etc/letsencrypt/live/usagecorpus.com/privkey.pem; # managed by Certbot
    include /etc/letsencrypt/options-ssl-nginx.conf; # managed by Certbot
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem; # managed by Certbot

}

```

```shell
sudo systemctl start nginx 

sudo systemctl restart nginx 

 

```

# ROUTER/MODEM LAN

```shell


 

ethernet 

 

wifi 


mac

ip
 

dns (check /etc/resolv.conf) 

 

dhcp (alongside static) 

# ethernet wan

 plug the cable into wan port

# ethernet hub/ap mode & ethernet extension

 turn off dhcp, enable hub/ap mode

 set internal new ip avoiding collision, under the default gateway cidr, set default gateway and dns

 plug the cable into lan port


# wireless wan

wifi wan

dont turn off dhcp

set internal new ip, for a new cidr range connect to the main ap with wisp


# wireless multibridge

wifi extension

set internal new ip avoiding collision, under the default gateway cidr

turn off dhcp

connect to the main ap with repeater


port forward 



```

# INTERFACE ADDRESS NETPLAN SYSTEMD-NETWORKD

```shell

# netplan

# below is the default configuration

# craete other ones if needed

```
```shell
# /etc/netplan/01-network-manager-all.yaml 

network:
 version: 2
 renderer: NetworkManager # or systemd-networkd
 ethernets:  # or wifis, bridges, modems
   eth0:
     dhcp4: no 
     addresses: [172.23.207.254/20]
     gateway4: 192.168.1.1
     nameservers:
         addresses: [8.8.8.8,8.8.8.4]
```
```shell
sudo netplan try

sudo netplan apply
```


```shell

# disable cloud init network config if necessary



```

```shell

sudo nano /etc/cloud/cloud.cfg.d/99-disable-network-config.cfg

network: {config: disabled}

```

# DISABLE/ENABLE WIFI IF

```shell
# disable
```

```shell
# /etc/network/interfaces
iface <interface_name> inet manual
```

```shell
# enable
# undo above

```

# IP INTERFACE IPTABLES NAT PORT FORWARD

```shell

# dummy

sudo modprobe dummy

sudo ip link add deth0 type dummy

sudo ip link set dev deth0 address C8:D7:4A:4E:47:50

sudo ip addr add 192.168.1.100/24 brd + dev deth0 # label deth0:0

sudo ip link set dev deth0 up

sudo ip link set dev deth0 down

sudo ip addr del 192.168.1.100/24 brd + dev deth0 # label deth0:0

sudo ip link delete deth0 type dummy

sudo modprobe -r dummy

```

```shell

# forward

# ephemeral

echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward

# permanent
cat <<EOF | sudo tee /etc/sysctl.d/99-ipv4-forward.conf
net.ipv4.ip_forward                 = 1
EOF

sudo sysctl -p

sudo sysctl --system

# init rule
sudo iptables -A FORWARD -i wlo1 -o deth0 -p tcp --syn --dport 8888 -m conntrack --ctstate NEW -j ACCEPT

# forward rules
sudo iptables -A FORWARD -i wlo1 -o deth0 -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT

sudo iptables -A FORWARD -i deth0 -o wlo1 -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT

# default DROP

sudo iptables -P FORWARD DROP

# routing rules

sudo iptables -t nat -A PREROUTING -i wlo1 -p tcp --dport 8888 -j DNAT --to-destination 192.168.1.100:8000

sudo iptables -t nat -A POSTROUTING -o deth0 -p tcp --dport 8000 -d 192.168.1.100 -j SNAT --to-source 192.168.50.24:8888

# permanent rule

sudo service netfilter-persistent save

# delete 

sudo iptables -S 

iptables -L -n -t nat

sudo iptables -D [ -t nat ] $A_SPEC

# or

sudo iptables -L --line-numbers

sudo iptables -D INPUT $LINE_NUM

```


# FIREWALL  

```shell


# ubuntu 



sudo ufw default allow incoming 

sudo ufw default allow outgoing 

 

sudo ufw allow ssh 

sudo ufw allow https 

 

sudo ufw allow 22 

sudo ufw allow 443 

 

sudo ufw allow 6000:6007/tcp 

 

sudo ufw allow from 203.0.113.4 to any port 22 

 

sudo ufw allow from 203.0.113.0/24 

 

sudo ufw deny from 203.0.113.4 

 

sudo ufw enable 

 

sudo ufw disable 

 

 

# centos 

 

systemctl enable firewalld 

 

sudo firewall-cmd --permanent --add-service={http,https} --permanent 

 

sudo firewall-cmd --add-port=7070/tcp --permanent 

 

firewall-cmd --zone=trusted --add-source=192.168.0.1/24 --permanent 

 

firewall-cmd --zone=trusted --add-source=10.10.0.0/16 --permanent 

```

# SSH KEY DISTRIBUTE

```shell
ssh-keygen -t rsa 

 

ssh-copy-id username@node_name 

```

# NCAT

```shell

apt install -y ncat 

 

ncat -lvp <port> 

 

ncat <addr> <port> -e /bin/bash
```


# AP

```shell
sudo apt-get install hostapd dnsmasq

sudo vim /etc/hostapd/hostapd.conf
```

```shell
# /etc/hostapd/hostapd.conf
interface=wlan0
driver=nl80211
ssid=MyWiFiNetwork
hw_mode=g
channel=7
wmm_enabled=0
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=12345678
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
```

```shell
sudo vim /etc/default/hostapd
```

```shell
# /etc/default/hostapd

DAEMON_CONF="/etc/hostapd/hostapd.conf"
```

```shell

sudo systemctl unmask hostapd
sudo systemctl enable hostapd
sudo systemctl start hostapd


sudo cp /etc/dnsmasq.conf /etc/dnsmasq.conf.org

sudo vim /etc/dnsmasq.conf

```

```shell
# /etc/dnsmasq.conf
port=5353
interface=wlan0
dhcp-range=192.168.4.2,192.168.4.20,255.255.255.0,24h
```
```shell

sudo systemctl enable hostapd
sudo systemctl start hostapd
sudo systemctl reload dnsmasq

sudo vim /lib/systemd/system/dnsmasq.service
```

```shell
# /lib/systemd/system/dnsmasq.service

After=network-online.target
Wants=network-online.target
```

```shell
sudo vim /etc/netplan/50-cloud-init.yaml
```

```yaml
network:
    ethernets:
        eth0:
            dhcp4: true
            optional: true
        wlan0:
            dhcp4: false
            addresses:
            - 192.168.4.1/24
    version: 2
```

```shell
sudo reboot
```

# SOCKET IO

```shell
# node
npm install socket.io
```

# GRPC

```shell
# golang

apt install -y protobuf-compiler

apt install golang-goprotobuf-dev -y

go install google.golang.org/protobuf/cmd/protoc-gen-go

go install google.golang.org/grpc/cmd/protoc-gen-go-grpc

protoc --go_out=plugins=grpc:. <file>.proto
```

# FS MOUNT

```shell


lsblk 

 

mkfs.<fs> <drive> 2>/dev/null 

 

mkfs.xfs /dev/sdb 2>/dev/null 

 

mkdir <dir> 

 

mount <drive> <dir> 

 

df 

 

/etc/fstab 

```


# RAID

```shell


mdadm --create --verbose /dev/md0 --level=1 --raid-devices=2 /dev/sdb /dev/sdc  

 

mkfs.<fs> <drive> 2>/dev/null 

 

mkdir <dir> 

 

mount <drive> <dir> 

 

df 

 

/etc/fstab 

```

# LVM DISK EXTENSION

```shell

# resize 

 

sudo lvremove /dev/ubuntu-box-SOMETHING 

 

sudo lvm lvextend -l +100%FREE /dev/ubuntu-box-1-vg 

 

sudo resize2fs -p /dev/ubuntu-box-1-vg 

 

# xfs 

sudo xfs_growfs /dev/ubuntu-box-1-vg 

 

# extend 

 

sudo pvcreate /dev/vdb 

 

sudo vgextend ubuntu-box-1-vg /dev/vdb 

 

sudo lvm lvextend -l +100%FREE /dev/ubuntu-box-1-vg 

 

sudo resize2fs -p /dev/ubuntu-box-1-vg 

 

# xfs 

sudo xfs_growfs /dev/ubuntu-box-1-vg 

 

/etc/fstab 

```

# NFS

```shell


# on nfs server 

 

sudo apt install nfs-kernel-server 

 

sudo apt install nfs-common 

 

mkdir -p /nfs/data 

 

sudo chown nobody:nogroup /nfs/data 

 

sudo vim /etc/exports   


```

```shell
# /etc/exports   


... 

/nfs/data    <client-addr>(rw,sync,no_subtree_check)  

 

```

```shell

exportfs -a 

 

sudo systemctl restart nfs-kernel-server  

 

# on nfs client 

 

sudo apt install nfs-common 

 

mkdir -p /nfsclient/upload 

 

sudo mount <nfs-server-addr>:/nfs/data /nfsclient/upload 

 

# disconnect  

 

sudo umount /nfsclient/upload 

 


```

# CHECK FILE SYSTEM FS INTEGRITY

```shell
fsck

```

# DD FORMAT DISK FDISK

```shell
# check disk

fdisk -l


# unmount

umount /dev/sdb


# format
sudo dd if=/dev/zero of=/dev/sdb bs=1m

# create new partition

fdisk
n

# write save

fdisk
w

# format iso on usb

sudo dd bs=4M if=filename.iso of=/dev/sdb status=progress

# format fs

sudo mkfs.vfat /dev/sdb1 

# eject

sudo eject /dev/sdb
```

# API KEY

```shell

# apt-key 

 

sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys <> 

 

# wget or curl 

 

sudo wget -q -O /usr/share/keyrings/<>.gpg https://<>/<>.gpg 

 

echo "deb [signed-by=/usr/share/keyrings/<>.gpg] https://packages.cloud.google.com/apt kubernetes-xenial" | sudo tee -a /etc/apt/sources.list.d/cloud-google.list 

 

```

# DB & SYNC

```shell



sudo apt install mysql-server mysql-client 

 

mysql_secure_installation 

 

# dump 

 

mysqldump -u username -p database_name > data-dump.sql 

 

mysql -u username -p new_database < data-dump.sql 

 

# sync 

 

# master 

 

sudo vim /etc/mysql/mysql.conf.d/mysqld.cnf 
```

```shell
bind-address 	 =0.0.0.0 

 

server-id	 =1 

... 

log_bin = /var/log/mysql/mysql-bin.log 

log_bin_index =/var/log/mysql/mysql-bin.log.index 

relay_log = /var/log/mysql/mysql-relay-bin 

relay_log_index = /var/log/mysql/mysql-relay-bin.index 
```

```shell

sudo systemctl restart mysql 

 

sudo mysql -u root -p 

 

CREATE USER 'replication_user'@'*' IDENTIFIED BY 'replica_password'; 

 

GRANT REPLICATION SLAVE ON *.* TO 'replication_user'@'*'; 

 

SHOW MASTER STATUS\G 

 

// Get File (MASTER_LOG_FILE), Position (MASTER_LOG_POS) 


# slave 

 

sudo vim  /etc/mysql/mysql.conf.d/mysqld.cnf 
```

```shell
# /etc/mysql/mysql.conf.d/mysqld.cnf 

bind-address           = 0.0.0.0 

 

server-id		=2  

 

log_bin = /var/log/mysql/mysql-bin.log 

log_bin_index =/var/log/mysql/mysql-bin.log.index 

relay_log = /var/log/mysql/mysql-relay-bin 

relay_log_index = /var/log/mysql/mysql-relay-bin.index 


```

```shell
 

sudo mysql -u root -p 

 

STOP SLAVE;  

 

CHANGE MASTER TO MASTER_HOST ='MASTER_IP', MASTER_USER ='replication_user', MASTER_PASSWORD ='replica_password', MASTER_LOG_FILE = 'mysql-bin.000002', MASTER_LOG_POS = 1643; 

 

START SLAVE; 

 

# MGR 

 

uuidgen 

 

sudo vim  /etc/mysql/mysql.conf.d/mysqld.cnf 

```

```shell

[mysqld] 

 

# General replication settings 

disabled_storage_engines="MyISAM,BLACKHOLE,FEDERATED,ARCHIVE,MEMORY" 

gtid_mode = ON 

enforce_gtid_consistency = ON 

master_info_repository = TABLE 

relay_log_info_repository = TABLE 

binlog_checksum = NONE 

log_slave_updates = ON 

log_bin = binlog 

binlog_format = ROW 

transaction_write_set_extraction = XXHASH64 

loose-group_replication_bootstrap_group = OFF 

loose-group_replication_start_on_boot = OFF 

loose-group_replication_ssl_mode = REQUIRED 

loose-group_replication_recovery_use_ssl = 1 

 

# Shared replication group configuration 

loose-group_replication_group_name = "" # uuidgen value 

loose-group_replication_ip_whitelist = "ip1,ip2,ip3" # group ips 

loose-group_replication_group_seeds = "ip1:port,ip2:port,ip3:port" # group ips with ports # port usually 33061 

 

# Single or Multi-primary mode? Uncomment these two lines 

# for multi-primary mode, where any host can accept writes 

#loose-group_replication_single_primary_mode = OFF  # for multi primary group uncomment 

#loose-group_replication_enforce_update_everywhere_checks = ON # for multi primary group uncomment 

 

# Host specific replication configuration 

server_id =    # id 

bind-address = "" # bind-address 

report_host = "" # bind-address  

loose-group_replication_local_address = "" # bind-address:port # port usually 33061 


```

```shell


systemctl restart mysql 

 

# on all members 

 

mysql 

 

SET SQL_LOG_BIN=0; 

 

CREATE USER 'repl'@'%' IDENTIFIED BY 'password' REQUIRE SSL; 

 

GRANT REPLICATION SLAVE ON *.* TO 'repl'@'%'; 

 

FLUSH PRIVILEGES; 

 

SET SQL_LOG_BIN=1; 

 

CHANGE REPLICATION SOURCE TO SOURCE_USER='repl', SOURCE_PASSWORD='password' FOR CHANNEL 'group_replication_recovery'; 

# or 

CHANGE MASTER TO MASTER_USER='repl', MASTER_PASSWORD='password' FOR CHANNEL 'group_replication_recovery'; 

 

INSTALL PLUGIN group_replication SONAME 'group_replication.so'; 

 

# on one member (maybe first) 

 

SET GLOBAL group_replication_bootstrap_group=ON; 

 

START GROUP_REPLICATION; 

 

SET GLOBAL group_replication_bootstrap_group=OFF; 

 

# on the other members 

 

START GROUP_REPLICATION; 

 

 
```

# MONGODB

```shell

curl -fsSL https://www.mongodb.org/static/pgp/server-4.4.asc | sudo apt-key add - 

 

echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu focal/mongodb-org/4.4 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-4.4.list 

 

sudo apt update 

 

sudo apt install mongodb-org 

 

mkdir -p ~/data/db 

 

sudo mongod --dbpath ~/data/db 


```


# CHROME

```shell

# download .deb 

 

sudo apt install ./<file>.deb 


```


# VSCODE

```shell
# download .deb 

 

sudo apt install ./<file>.deb 
```

# GOLANG

```shell
curl -OL https://golang.org/dl/go1.16.7.linux-amd64.tar.gz 

 

sudo tar -C /usr/local -xvf go1.16.7.linux-amd64.tar.gz 

 

sudo nano ~/.profile 


```

```shell
. . . 

export PATH=$PATH:/usr/local/go/bin 
```

```shell
source ~/.profile 

```

# KOTLIN

```shell


sudo apt install openjdk-11-jdk 

 

curl -s https://get.sdkman.io | bash 

 

sdk install kotlin 

 

kotlinc Kotlin.kt -include-runtime -d outPutFile.jar 

 

java -jar outPutFile.jar 

```

# NODEJS

```shell

curl -sL https://deb.nodesource.com/setup_16.x -o /tmp/nodesource_setup.sh 

 

sudo bash /tmp/nodesource_setup.sh 

 

sudo apt install nodejs 



```


# DJANGO

```shell

django-admin startproject mysite

```

# EXPRESSJS

```shell

 npm install -g express-generator
 
 express myapp

```

# JMETER

```shell

# install openjdk

# create thread group

# add sampler

jmeter -n -t <ThreadGroup.jmx> -l <report.csv> -e -o <report>

```

# CRYPTO

```shell

openssl


# issuer(ca) keygen

openssl genrsa -out ca_priv.pem 2048

openssl rsa -in ca_priv.pem -outform PEM -pubout -out ca_pub.pem

openssl req -x509 -new -key ca_priv.pem -days 365 -out ca.crt -subj "/CN=issuerforseantywork.com"

# subject(issuer) keygen

openssl genrsa -out sub_priv.pem 2048

openssl rsa -in sub_priv.pem -outform PEM -pubout -out sub_pub.pem

# subject csr

openssl req -key sub_priv.pem -new -sha256 -out sub.csr

# issuer signing

openssl  x509 -req -days 180 -in sub.csr -CA ca.crt -CAkey ca_priv.pem -CAcreateserial -sha256 -out sub.crt

# issuer signing with sans

openssl  x509 -req -extfile <(printf "subjectAltName = DNS:some.thing") -days 180 -in sub.csr -CA ca.crt -CAkey ca_priv.pem -CAcreateserial -sha256 -out sub.crt

# read csr, certificate

openssl x509 -in <csr,crt> -text -noout

# verify subject cert against issuer cert

openssl verify -CAfile ca.crt sub.crt


# x509

 certificate and public key send, root ca on the authenticate requester side, chained authentication

 if done, use the pub key to send symmetric key

# gpg

gpg --output encrypted.data --symmetric --cipher-algo AES256 un_encrypted.data

gpg --output un_encrypted.data --decrypt encrypted.data


# john

./john --format=raw-MD5-opencl --wordlist=../passwords.txt --rules ../md5.txt

# hashcat

./hashcat.bin -m 17400 ../sha3_256.txt ../passwords.txt -r ../password_rule.rule -w 3 --force



```

# DOCKER & KUBERNETES

```shell


docker version - 20.10.17 

kubectl version v1.24.3 

 

# !!!! sudo -i  FIRST!!!! 

 

# swap disabled and firewall check 

swapoff -a 

apt update 

apt upgrade -y 


```


# DOCKER INSTALL

```shell


apt remove docker docker-engine docker.io containerd runc 

apt update 

apt install \ 

    ca-certificates \ 

    curl \ 

    gnupg \ 

    lsb-release 

mkdir -p /etc/apt/keyrings 

curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg 

echo \ 

  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \ 

  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null 

apt update 

apt install docker-ce docker-ce-cli containerd.io docker-compose-plugin 

 
```

# INSTALL DOCKER-COMPOSE

```shell

curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose 

chmod +x /usr/local/bin/docker-compose 

 

```

# PREPARE DOCKER IMAGE

```shell

cat > /etc/docker/daemon.json <<EOF 

{ 

  "exec-opts": ["native.cgroupdriver=systemd"], 

  "log-driver": "json-file", 

  "log-opts": { 

    "max-size": "100m" 

  }, 

  "storage-driver": "overlay2", 

  "storage-opts": [ 

    "overlay2.override_kernel_check=true" 

  ] 

} 

EOF 

systemctl restart docker 

# add runtime: sysbox-runc   if using sysbox



docker-compose up  

 

docker-compose down 

 

docker login 

 

docker tag IMAGEID seantywork/NAME 

 

docker push seantywork/NAME

```

# DOCKER PRIVATE

```shell

docker login <address:port> 
```


# DOCKER 

```shell

docker image 
docker container
docker network
docker volume
 
```


# DOCKER-COMPOSE

```shell

# build, name, port,  

# image , volume, runtime 

```


# DOCKER REMOTE

```shell



# on docker daemon host 

 

ssh-keygen 

 

# move id_rsa to the client 

# add id_rsa.pub to authrized_keys 

# on client move id_rsa to ~/.ssh 

 

docker -H ssh://user@host <command> 


```

# DOCKER REMOTE CONTEXT

```shell


docker context create \ 

    --docker host=ssh://user@host \ 

    docker-remote 

 

docker context use docker-remote 

 

docker context use default 

 
```

# KUBERNETES INSTALL


```shell

apt update 

 

apt install -y apt-transport-https ca-certificates curl 

 

curl -fsSLo /usr/share/keyrings/kubernetes-archive-keyring.gpg https://packages.cloud.google.com/apt/doc/apt-key.gpg 

 

echo "deb [signed-by=/usr/share/keyrings/kubernetes-archive-keyring.gpg] https://apt.kubernetes.io/ kubernetes-xenial main" | sudo tee /etc/apt/sources.list.d/kubernetes.list 

 

apt update 

 

apt install -y kubectl kubelet kubeadm  

 

sudo apt-mark hold kubelet kubeadm kubectl 


```


# KIND INSTALL

```shell


curl -Lo ./kind "https://kind.sigs.k8s.io/dl/v0.14.0/kind-$(uname)-amd64" 

 

chmod +x ./kind 

 

sudo mv ./kind /usr/local/bin/ 

```


# KUBEADM ALL NODES


```shell

#!/bin/bash 

 

 

 

set -euxo pipefail 

 

# Variable Declaration 

 

KUBERNETES_VERSION="1.23.6-00" 

 

# disable swap 

sudo swapoff -a 

 

# keeps the swaf off during reboot 

(crontab -l 2>/dev/null; echo "@reboot /sbin/swapoff -a") | crontab - || true 

sudo apt-get update -y 

 

 

# Install CRI-O Runtime 

 

OS="xUbuntu_20.04" 

 

VERSION="1.23" 

 

# Create the .conf file to load the modules at bootup 

cat <<EOF | sudo tee /etc/modules-load.d/crio.conf 

overlay 

br_netfilter 

EOF 

 

sudo modprobe overlay 

sudo modprobe br_netfilter 

 

# Set up required sysctl params, these persist across reboots. 

cat <<EOF | sudo tee /etc/sysctl.d/99-kubernetes-cri.conf 

net.bridge.bridge-nf-call-iptables  = 1 

net.ipv4.ip_forward                 = 1 

net.bridge.bridge-nf-call-ip6tables = 1 

EOF 

 

sudo sysctl --system 

 

cat <<EOF | sudo tee /etc/apt/sources.list.d/devel:kubic:libcontainers:stable.list 

deb https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/$OS/ / 

EOF 

cat <<EOF | sudo tee /etc/apt/sources.list.d/devel:kubic:libcontainers:stable:cri-o:$VERSION.list 

deb http://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable:/cri-o:/$VERSION/$OS/ / 

EOF 

 

curl -L https://download.opensuse.org/repositories/devel:kubic:libcontainers:stable:cri-o:$VERSION/$OS/Release.key | sudo apt-key --keyring /etc/apt/trusted.gpg.d/libcontainers.gpg add - 

curl -L https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/$OS/Release.key | sudo apt-key --keyring /etc/apt/trusted.gpg.d/libcontainers.gpg add - 

 

sudo apt-get update 

sudo apt-get install cri-o cri-o-runc -y 

 

sudo systemctl daemon-reload 

sudo systemctl enable crio --now 

 

echo "CRI runtime installed susccessfully" 

 

# Install kubelet, kubectl and Kubeadm 

 

sudo apt-get update 

sudo apt-get install -y apt-transport-https ca-certificates curl 

sudo curl -fsSLo /usr/share/keyrings/kubernetes-archive-keyring.gpg https://packages.cloud.google.com/apt/doc/apt-key.gpg 

 

echo "deb [signed-by=/usr/share/keyrings/kubernetes-archive-keyring.gpg] https://apt.kubernetes.io/ kubernetes-xenial main" | sudo tee /etc/apt/sources.list.d/kubernetes.list 

sudo apt-get update -y 

sudo apt-get install -y kubelet="$KUBERNETES_VERSION" kubectl="$KUBERNETES_VERSION" kubeadm="$KUBERNETES_VERSION" 

sudo apt-get update -y 

sudo apt-get install -y jq 

 

 

netinf_row="$(ip -o link | awk '$2 != "lo:" {print $2, $(NF-2)}')" 

searchstring=": " 

 

rest=${netinf_row#*$searchstring} 

idx=$(( ${#netinf_row} - ${#rest} - ${#searchstring} )) 

netinf_name=${netinf_row:0:idx} 

 

local_ip="$(ip --json a s | jq -r --arg NETINF_NAME $netinf_name '.[] | if .ifname == $NETINF_NAME then .addr_info[] | if .family == "inet" then .local else empty end else empty end')" 

cat > /etc/default/kubelet << EOF 

KUBELET_EXTRA_ARGS=--node-ip=$local_ip 

EOF 

 


```

# MASTER - ON MASTER NODE


```shell

set -euxo pipefail 

 

netinf_row="$(ip -o link | awk '$2 != "lo:" {print $2, $(NF-2)}')" 

searchstring=": " 

 

rest=${netinf_row#*$searchstring} 

idx=$(( ${#netinf_row} - ${#rest} - ${#searchstring} )) 

netinf_name=${netinf_row:0:idx} 

 

local_ip="$(ip --json a s | jq -r --arg NETINF_NAME $netinf_name '.[] | if .ifname == $NETINF_NAME then .addr_info[] | if .family == "inet" then .local else empty end else empty end')" 

 

MASTER_IP=$local_ip 

#LB_IP=<load_balancer_ip> 

#PORT=<load_balancer_port> 

NODENAME=$(hostname -s) 

POD_CIDR="10.10.0.0/16" 

 

sudo kubeadm config images pull 

 

echo "Preflight Check Passed: Downloaded All Required Images" 

 

sudo kubeadm init --apiserver-advertise-address=$MASTER_IP --apiserver-cert-extra-sans=$MASTER_IP --pod-network-cidr=$POD_CIDR --node-name "$NODENAME" --ignore-preflight-errors Swap  

 

mkdir -p "$HOME"/.kube 

sudo cp -i /etc/kubernetes/admin.conf "$HOME"/.kube/config 

sudo chown "$(id -u)":"$(id -g)" "$HOME"/.kube/config 

 

 

 

curl https://raw.githubusercontent.com/projectcalico/calico/v3.25.0/manifests/calico.yaml -O 

 

kubectl apply -f calico.yaml 

 

 

mkdir -p "$HOME"/.kube 

sudo cp -i /etc/kubernetes/admin.conf "$HOME"/.kube/config 

sudo chown "$(id -u)":"$(id -g)" "$HOME"/.kube/config 

 

# Install Claico Network Plugin Network  

 

curl https://raw.githubusercontent.com/projectcalico/calico/v3.25.0/manifests/calico.yaml -O 

 

kubectl apply -f calico.yaml 


```


# JOIN TOKEN CREATION

```shell
kubeadm token create --print-join-command 

```


# WORKER NODE JOIN

```shell

sudo kubeadm join 10.128.0.37:6443 --token j4eice.33vgvgyf5cxw4u8i \ 

    --discovery-token-ca-cert-hash sha256:37f94469b58bcc8f26a4aa44441fb17196a585b37288f85e22475b00c36f1c61 

 

```


# LABEL WORKER


```shell
sudo kubectl label node worker-node01  node-role.kubernetes.io/worker=worker 

 

sudo kubectl label node worker-node01  ingress-ready=true 

 


```


# IN CASE OF DNS MESSED UP

```shell


sudo kubectl -n kube-system rollout restart deployment coredns 

 

```


# MASTER NODE SCHEDULING


```shell
kubectl taint nodes --all node-role.kubernetes.io/master- 

# or

sudo kubectl taint node <master> node-role.kubernetes.io/control-plane:NoSchedule-

sudo kubectl taint node <master> node-role.kubernetes.io/master:NoSchedule-


```

# PREVENT SCHEDULING


```shell
sudo kubectl cordon <node> 

 

```


# TEAR DOWN CLUSTER


```shell

sudo kubectl drain <node> --delete-local-data --force --ignore-daemonsets 

 
```


# RE-SCHEDULE NODE

```shell
sudo kubectl uncordon <node> 

```

# ON EACH NODE

```shell

sudo kubeadm reset 
```

# HA CLUSTER


```shell

sudo apt-get update 

sudo apt-get install haproxy 

 

sudo vim /etc/haproxy/haproxy.cfg 

```

```shell
# /etc/haproxy/haproxy.cfg 



global 

... 

default 

... 

frontend kubernetes 

bind <kube-lb-domain-or-ip>:<kube-lb-port> # *:<kube-lb-port> 

option tcplog 

mode tcp 

#default_backend kubernetes-master-nodes 

 

 

#acl kubernetes-master-rule hdr(host) -i k8s.cluster.com 

 

 

#use_backend kubernetes-master-nodes if kubernetes-master-rule 

 

 

backend kubernetes-master-nodes 

mode tcp 

balance roundrobin 

option tcp-check 

server k8s-master-0 <kube-master-0>:6443 check fall 3 rise 2 

server k8s-master-1 <kube-master-1>:6443 check fall 3 rise 2 

server k8s-master-2 <kube-master-2>:6443 check fall 3 rise 2 

```

```shell


sudo systemctl restart haproxy 

 

 

# same wj.sh on all nodes 

 

# different mj.sh on the first master node 

 

sudo kubeadm init --apiserver-advertise-address=$MASTER_IP --apiserver-cert-extra-sans="$MASTER_IP,$KUBELBADDR" --pod-network-cidr=$POD_CIDR --control-plane-endpoint "$KUBELBADDR:$KUBELBPORT"  --node-name "$NODENAME" --ignore-preflight-errors Swap 

 
```

# SCP CERTS


```shell


USER=<user-on-host> # customizable 

# Set the control_plane_ips to all other master node ips or hostnames 

CONTROL_PLANE_IPS="<addr1> <addr2>" 

for host in ${CONTROL_PLANE_IPS}; do 

    scp /etc/kubernetes/pki/ca.crt "${USER}"@$host: 

    scp /etc/kubernetes/pki/ca.key "${USER}"@$host: 

    scp /etc/kubernetes/pki/sa.key "${USER}"@$host: 

    scp /etc/kubernetes/pki/sa.pub "${USER}"@$host: 

    scp /etc/kubernetes/pki/front-proxy-ca.crt "${USER}"@$host: 

    scp /etc/kubernetes/pki/front-proxy-ca.key "${USER}"@$host: 

    scp /etc/kubernetes/pki/etcd/ca.crt "${USER}"@$host:etcd-ca.crt 

    scp /etc/kubernetes/pki/etcd/ca.key "${USER}"@$host:etcd-ca.key 

    scp /etc/kubernetes/admin.conf "${USER}"@$host: 

done 

 


```


# MV CERTS ON EACH MASTER NODE


```shell


USER=<user-on-host> # customizable 

mkdir -p /etc/kubernetes/pki/etcd 

mv /home/${USER}/ca.crt /etc/kubernetes/pki/ 

mv /home/${USER}/ca.key /etc/kubernetes/pki/ 

mv /home/${USER}/sa.pub /etc/kubernetes/pki/ 

mv /home/${USER}/sa.key /etc/kubernetes/pki/ 

mv /home/${USER}/front-proxy-ca.crt /etc/kubernetes/pki/ 

mv /home/${USER}/front-proxy-ca.key /etc/kubernetes/pki/ 

mv /home/${USER}/etcd-ca.crt /etc/kubernetes/pki/etcd/ca.crt 

mv /home/${USER}/etcd-ca.key /etc/kubernetes/pki/etcd/ca.key 

mv /home/${USER}/admin.conf /etc/kubernetes/admin.conf 

 

```


# JOIN MASTER NODE


```shell


sudo kubeadm join 10.128.0.37:6443 --token j4eice.33vgvgyf5cxw4u8i \ 

    --discovery-token-ca-cert-hash sha256:37f94469b58bcc8f26a4aa44441fb17196a585b37288f85e22475b00c36f1c61 \ 

    --control-plane --node-name <name> 

 

# admin.conf -> .kube/config 
```


# JOIN WORKER NODE


```shell
sudo kubeadm join 10.128.0.37:6443 --token j4eice.33vgvgyf5cxw4u8i \ 

    --discovery-token-ca-cert-hash sha256:37f94469b58bcc8f26a4aa44441fb17196a585b37288f85e22475b00c36f1c61 \ 

    --node-name <name> 

 


```

# CNIS


```shell

kubectl apply -f  <CNI> ex) https://docs.projectcalico.org/manifests/calico.yaml , "https://cloud.weave.works/k8s/net?k8s-version=$(kubectl version | base64 | tr -d '\n')" , https://raw.githubusercontent.com/aojea/kindnet/master/install-kindnet.yaml 

 
```


# KIND RUN


```yaml

# Kind yaml structure example



kindcluster.yaml 

 

kind: Cluster 

 

apiVersion: kind.x-k8s.io/v1alpha4 

 

nodes: 

 

 - role: control-plane 

     kubeadmConfigPatches: 

  - | 

    kind: InitConfiguration 

    nodeRegistration: 

      kubeletExtraArgs: 

        node-labels: "ingress-ready=true" 

  extraPortMappings: 

  - containerPort: 80 

    hostPort: 80 

    protocol: TCP 

  - containerPort: 443 

    hostPort: 443 

    protocol: TCP 

 - role: worker 

 

 - role: worker
```

```shell
kind create cluster --name kindcluster --config ./kindcluster.yaml --image=kindest/node:v1.21.12 

 
```

# KUBECTL DEPLOYMENT


```shell
kubectl apply -f <yamls> 
```

# PORT FORWARD

```shell
kubectl port-forward resource/name port
```


# KUBECTL INGRESS


```shell
kubectl label node <node> ingress-ready=true 

 

kubectl apply -f <controller> ex) https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml 

 

kubectl apply -f ingress.yaml 


```

# TLS INGRESS NEED SECRET


```shell


# self singed certs 

 

openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout ./tls.key -out ./tls.crt -subj "/CN=localhost" 

 

kubectl create secret tls --save-config tls-openssl --key ./tls.key --cert ./tls.crt 

 

# include $HOME .kube config -- remote kubectl and switching 

 

kubectl config use-context <context-name

```


# TURNING OFF

```shell

kubectl delete --all ingress, services, deployments 

 

kind delete cluster --name NAME 

 

docker system prune -a  

```


# CHECK


```shell
systemctl status docker 

systemctl status kubelet 

```

# SECRET REG


```shell

kubectl create secret generic <regcred> \ 

    --from-file=.dockerconfigjson=<path/to/.docker/config.json> \ 

    --type=kubernetes.io/dockerconfigjson 

 

kubectl create secret docker-registry <pull_secret_name> \ 

    --docker-server=<registry_server> \ 

    --docker-username=<user_name> \ 

    --docker-password=<password> \ 

```


# K8S COMPONENTS

```shell


ingress controller -> ingress -> svc 

svc -> dpl, stf  

secret -> tls, reg -> cert&key, credentials 

vol -> pv[k8sdiskpath], storageclass -> stf, volmount[containerpath] = volclaim = storageclass 

```

# DEPLOYMENT EXAMPLE


```yaml
apiVersion: apps/v1 

kind: Deployment 

metadata: 

  name: db 

spec: 

  selector: 

    matchLabels: 

      app: db 

  replicas: 1 

  template: 

    metadata: 

      labels: 

        app: db 

    spec: 

      imagePullSecrets: 

        - name: docker-secret 

      containers: 

        - name: db 

          image: seantywork/db 

          ports: 

          - containerPort: 3306 

```


# SERVICE EXAMPLE


```yaml

apiVersion: v1 

kind: Service 

metadata: 

  name: db 

  labels: 

    app: db 

spec: 

  type: ClusterIP 

  ports: 

  - port: 3306 

    targetPort: 3306 

    protocol: TCP 

  selector: 

    app: db 

```


# SERVICE IN DIFFERENT NAMESPACE


```shell

<serivce_name>.<namespace_name> 
```

# NODE SELECTOR & AFFINITY


```yaml


apiVersion: v1 

kind: Pod 

 

.... 

 

spec: 

  nodeSelector:  

    region: east 

    type: user-node 

```


```yaml

apiVersion: v1 

kind: Pod 

metadata: 

  name: with-node-affinity 

spec: 

  affinity: 

    nodeAffinity: 

      requiredDuringSchedulingIgnoredDuringExecution: 

        nodeSelectorTerms: 

        - matchExpressions: 

          - key: topology.kubernetes.io/zone 

            operator: In 

            values: 

            - antarctica-east1 

            - antarctica-west1 

 


```


# ENV


```yaml
apiVersion: apps/v1 

kind: Deployment 

 

.... 

 

 

    spec: 

      containers: 

        - name: kibana 

          image: docker.elastic.co/kibana/kibana:7.11.0 

          env: 

          - name: ELASTICSEARCH_HOSTS 

            value: http://elasticsearch:9200 

          imagePullPolicy: Always 

          ports: 

          - containerPort: 5601 


```


# COMMAND


```yaml

.... 

 

containers: 

  - name: mysqldump 

    image: mysql 

    command: ["/bin/sh", "-c"] 

    args: 

      - echo starting; 

        ls -la /backups; 

        mysqldump --host=... -r /backups/file.sql db_name; 

        ls -la /backups; 

        echo done; 

    volumeMounts: 

      - ... 

 

```


# CONFIGMAP

```yaml


apiVersion: v1 

kind: ConfigMap 

metadata: 

  namespace: logging 

  name: elasticsearch-data-config 

  labels: 

    app: elasticsearch 

    role: data 

data: 

  elasticsearch.yml: |- 

    cluster.name: ${CLUSTER_NAME} 

    node.name: ${NODE_NAME} 

    discovery.seed_hosts: ${NODE_LIST} 

    cluster.initial_master_nodes: ${MASTER_NODES} 

    network.host: 0.0.0.0 

    node: 

      master: false 

      data: true 

      ingest: false 

    xpack.security.enabled: true 

    xpack.monitoring.collection.enabled: true 


```

# NODEPORT


```yaml
kind: Service
apiVersion: v1
metadata:
  name: nodeport-test
spec:
  type: NodePort
  selector:
    app: tgtraffic
  ports:
    - nodePort: 30505
      port: 5005
      targetPort: 5005

```

# INGRESS

```yaml

apiVersion: networking.k8s.io/v1 

kind: Ingress 

metadata: 

  name: ingress 

spec: 

  tls: 

  - hosts: 

    - usagecorpus.com 

    secretName: uc-tls-secret 

  - hosts: 

    - www.usagecorpus.com 

    secretName: uc-tls-secret 

  - hosts: 

    - ptt.usagecorpus.com 

    secretName: ptt-tls-secret 

  rules: 

  - host: "usagecorpus.com" 

    http: 

      paths: 

        - path: / 

          pathType: Prefix 

          backend: 

            service: 

              name: traffic 

              port: 

                number: 5005 

  - host: "www.usagecorpus.com" 

    http: 

      paths: 

        - path: / 

          pathType: Prefix 

          backend: 

            service: 

              name: traffic 

              port: 

                number: 5005 

  - host: "ptt.usagecorpus.com" 

    http: 

      paths: 

        - path: / 

          pathType: Prefix 

          backend: 

            service: 

              name: trafficph 

              port: 

                number: 5006 


```


# CONFIGMAP


```yaml

apiVersion: v1
data:
  allow-snippet-annotations: "true"
  use-forwarded-headers: "true"
  compute-full-forwarded-for: "true"
  keep-alive: "20"
kind: ConfigMap
metadata:
  annotations:
    kubectl.kubernetes.io/last-applied-configuration: |
      {"apiVersion":"v1","data":{"allow-snippet-annotations":"true","use-forwarded-headers":"true"},"kind":"ConfigMap","metadata":{"annotations":{},"creationTimestamp":"2022-11-23T07:56:59Z","labels":{"app.kubernetes.io/component":"controller","app.kubernetes.io/instance":"ingress-nginx","app.kubernetes.io/name":"ingress-nginx","app.kubernetes.io/part-of":"ingress-nginx","app.kubernetes.io/version":"1.5.1"},"name":"ingress-nginx-controller","namespace":"ingress-nginx","resourceVersion":"383","uid":"08673e9a-bdba-4a5a-9dda-a7f470d6214e"}}
  creationTimestamp: "2022-11-23T07:56:59Z"
  labels:
    app.kubernetes.io/component: controller
    app.kubernetes.io/instance: ingress-nginx
    app.kubernetes.io/name: ingress-nginx
    app.kubernetes.io/part-of: ingress-nginx
    app.kubernetes.io/version: 1.5.1
  name: ingress-nginx-controller
  namespace: ingress-nginx
  resourceVersion: "24225834"
  uid: 08673e9a-bdba-4a5a-9dda-a7f470d6214e
```


# SECRET EXAMPLE (TLS)

```shell
apiVersion: v1 

data: 

  tls.crt: ./tls.crt 

  tls.key: ./tls.key 

kind: Secret 

metadata: 

  name: uc-tls-secret 

  namespace: default 

type: kubernetes.io/tls 

```

# ANNOTATION

```yaml

apiVersion: networking.k8s.io/v1 

kind: Ingress 

metadata: 

  name: minimal-ingress 

  annotations: 
    nginx.ingress.kubernetes.io/proxy-body-size: "0"
    nginx.ingress.kubernetes.io/proxy-http-version : "1.1"
    nginx.ingress.kubernetes.io/proxy-request-buffering: "off"
    nginx.ingress.kubernetes.io/proxy-buffering: "off"
    nginx.ingress.kubernetes.io/rewrite-target: / 

spec: 

  ingressClassName: nginx-example 

  rules: 

  - http: 

      paths: 

      - path: /testpath 

        pathType: Prefix 

        backend: 

          service: 

            name: test 

            port: 

              number: 80 


```

# DEPLOYMENT EXAMPLE (VOLUME)


```yaml

apiVersion: apps/v1 

kind: Deployment 

metadata: 

  name: db 

spec: 

  selector: 

    matchLabels: 

      app: db 

  replicas: 1 

  template: 

    metadata: 

      labels: 

        app: db 

    spec: 

      imagePullSecrets: 

        - name: docker-secret 

      volumes: 

        - name: db-pv-storage 

          persistentVolumeClaim: 

            claimName: dbpvc 

      containers: 

        - name: db 

          image: seantywork/db 

          ports: 

          - containerPort: 3306 

          volumeMounts: 

          - mountPath: "/var/lib/mysql" 

            name: db-pv-storage 

```

# STATEFULSET EXAMPLE


```yaml
apiVersion: apps/v1 

kind: StatefulSet 

metadata: 

  name: dbstf 

spec: 

  selector: 

    matchLabels: 

      app: dbstf 

  serviceName: "dbstf" 

  replicas: 1 

  template: 

    metadata: 

      labels: 

        app: dbstf 

    spec: 

      terminationGracePeriodSeconds: 10 

      containers: 

      - name: dbstf 

        image: seantywork/db 

        ports: 

        - containerPort: 3306 

        volumeMounts: 

        - name: dbpvc 

          mountPath: /var/lib/mysql 

  volumeClaimTemplates: 

  - metadata: 

      name: dbpvc 

    spec: 

      accessModes: ["ReadWriteOnce"] 

      storageClassName: "manual" 

      resources: 

        requests: 

          storage: 3Gi 


```

# STORAGECLASS EXAMPLE (DEFAULT)

```shell

kind: StorageClass 

apiVersion: storage.k8s.io/v1 

metadata: 

  annotations: 

    storageclass.kubernetes.io/is-default-class: "true" 

  name: pv2-local-storageclass 

provisioner: kubernetes.io/no-provisioner 

volumeBindingMode: WaitForFirstConsumer 

```


# STORAGECLASS EXAMPLE (DEFAULT NFS)

```shell

helm repo add nfs-subdir-external-provisioner https://kubernetes-sigs.github.io/nfs-subdir-external-provisioner/ 

 

helm install nfs-subdir-external-provisioner nfs-subdir-external-provisioner/nfs-subdir-external-provisioner \ 

    --set nfs.server=x.x.x.x \ 

    --set nfs.path=/exported/path 

# yaml
```

```yaml
kind: StorageClass 

apiVersion: storage.k8s.io/v1 

metadata: 

  annotations: 

    storageclass.kubernetes.io/is-default-class: "true" 

  name: nfs-default-storageclass 

provisioner: k8s-sigs.io/nfs-subdir-external-provisioner # cluster.local/nfs-subdir-external-provisioner 

allowVolumeExpansion: true 

volumeBindingMode: Immediate 

```

# PERSISTENT VOLUME EXAMPLE (HOSTPATH)

```yaml

apiVersion: v1 

kind: PersistentVolume 

metadata: 

  name: dbpv 

  labels: 

    type: local 

spec: 

  storageClassName: manual 

  capacity: 

    storage: 10Gi 

  accessModes: 

    - ReadWriteOnce 

  hostPath: 

    path: "/home/styw/project/k8s/vol_host" 

```

# PERSISTENT VOLUME EXAMPLE (LOCALSTORAGE) 

```yaml

apiVersion: v1 

kind: PersistentVolume 

metadata: 

  name: pv1-local 

spec: 

  capacity: 

    storage: 64Gi 

  accessModes: 

  - ReadWriteOnce 

  persistentVolumeReclaimPolicy: Recycle 

  storageClassName: local-storage 

  local: 

    path: /data/volumes/pv1 

  nodeAffinity: 

    required: 

      nodeSelectorTerms: 

      - matchExpressions: 

        - key: kubernetes.io/hostname 

          operator: In 

          values: 

          - worker1-lw 

```

# PERSISTENT VOLUME EXAMPLE (NFS) 

```yaml
apiVersion: v1 

kind: PersistentVolume 

metadata: 

  name: dbpv 

spec: 

  capacity: 

    storage: 10Gi 

  volumeMode: Filesystem 

  accessModes: 

    - ReadWriteMany 

  persistentVolumeReclaimPolicy: Recycle 

  storageClassName: nfs 

  mountOptions: 

    - hard 

    - nfsvers=4.1 

  nfs: 

    path: /opt/k8s-pods/data 

    server: 192.168.1.40 


```

# PERSISTENT VOLUME CLAIM EXAMPLE (HOSTPATH) 

```yaml

apiVersion: v1 

kind: PersistentVolumeClaim 

metadata: 

  name: dbpvc 

spec: 

  storageClassName: manual 

  accessModes: 

    - ReadWriteOnce 

  resources: 

    requests: 

      storage: 3Gi 

```

# PERSISTENT VOLUME CLAIM EXAMPLE (LOCALSTORAGE) 

```yaml
kind: PersistentVolumeClaim 

apiVersion: v1 

metadata: 

  name: pv1-local-claim 

spec: 

  accessModes: 

  - ReadWriteOnce 

  storageClassName: local-storage 

  resources: 

    requests: 

      storage: 64Gi 

```

# PERSISTENT VOLUME CLAIM EXAMPLE (NFS) 

```yaml

apiVersion: v1 

kind: PersistentVolumeClaim 

metadata: 

  name: dbpvc 

spec: 

  storageClassName: nfs 

  accessModes: 

    - ReadWriteMany 

  resources: 

    requests: 

      storage: 10Gi 

```

# SYSBOX - FOR DIND 

```shell


# install 

wget https://downloads.nestybox.com/sysbox/releases/v0.5.0/sysbox-ce_0.5.0-0.linux_amd64.deb 

 

apt install jq 

apt install ./sysbox-ce_0.5.0-0.linux_amd64.deb 

 

systemctl status docker 

systemctl status sysbox 

 

docker-compose.yaml -- add runtime: sysbox-runc

```

# ON K8S

```shell
kubectl label node <node-name> sysbox-install=yes 

kubectl apply -f https://raw.githubusercontent.com/nestybox/sysbox/master/sysbox-k8s-manifests/sysbox-install.yaml 

 

```

# REMOTE KUBECTL

```shell



kubectl -n kube-system create serviceaccount <service-account-name> 

 

kubectl create clusterrolebinding <clusterrolebinding-name> --clusterrole=cluster-admin --serviceaccount=kube-system:<service-account-name> 

 

export TOKENNAME=$(kubectl -n kube-system get serviceaccount/<service-account-name> -o jsonpath='{.secrets[0].name}') 

 

export TOKEN=$(kubectl -n kube-system get secret $TOKENNAME -o jsonpath='{.data.token}' | base64 --decode) 

 

curl -k -H "Authorization: Bearer $TOKEN" -X GET "https://<KUBE-API-IP>:6443/api/v1/nodes" | json_pp 

 

kubectl config set-credentials <service-account-name> --token=$TOKEN 

 

kubectl config set-context --current --user=<service-account-name> 

 

# erase TLS auth info and add to cluster the option insecure-skip-tls-verify: true 

 

# then copy and paste the $HOME .kube config and modify server address 

 

# or just copy the whole config with a correct API server address 

 

```


# KUBE PROXY

```shell

kubectl proxy --address 0.0.0.0 --accept-hosts '.*' --port=<port> 

 
```

# KUBECTL REMOTE WITH TLS AND CUSTOM DOMAIN, IP

```shell

kubectl -n kube-system get configmap kubeadm-config -o jsonpath='{.data.ClusterConfiguration}' > kubeadm.yaml 

 

# edit certSANS 

 

# on control plane 

# remove below 

 

/etc/kubernetes/pki/apiserver.{crt,key} ~ 

 

kubeadm init phase certs apiserver --config kubeadm.yaml 

 

docker ps | grep kube-apiserver | grep -v pause 

 

docker kill <containerID> 

 

# or 

 

crictl pods | grep kube-apiserver | cut -d' ' -f1 

 

crictl stopp <pod-id> 

 

crictl rmp <pod-id> 

 

# wait till kube-system pods to restart and accessible by the dns 

# or

# on all master nodes

# remove current apiserver certificates
sudo rm /etc/kubernetes/pki/apiserver.*

# generate new certificates
sudo kubeadm init phase certs apiserver --apiserver-cert-extra-sans="ADDR1,ADDR2,ADDR3"
 


```

# KUBEADM RENEW CERTIFICATE

```shell

# on all control plane nodes

kubeadm certs renew all

# then restart

mv /etc/kubernetes/manifest/* mv /etc/kubernetes/manifest_tmp

# wait 20+ seconds

mv /etc/kubernetes/manifest_tmp/* mv /etc/kubernetes/manifest


```

# KUBEADM UPGRADE

```shell

# on the first master nodes

sudo apt-mark unhold kubeadm && \
sudo apt-get update && sudo apt-get install -y kubeadm=1.27.x-00 && \
sudo apt-mark hold kubeadm

sudo kubeadm upgrade apply v1.27.x

sudo kubectl drain <node-to-drain> --ignore-daemonsets

sudo apt-mark unhold kubelet kubectl && \
sudo apt-get update && sudo apt-get install -y kubelet=1.27.x-00 kubectl=1.27.x-00 && \
sudo apt-mark hold kubelet kubectl

sudo systemctl daemon-reload
sudo systemctl restart kubelet

sudo kubectl uncordon <node-to-uncordon>

# on other master nodes 

sudo apt-mark unhold kubeadm && \
sudo apt-get update && sudo apt-get install -y kubeadm=1.27.x-00 && \
sudo apt-mark hold kubeadm

sudo kubeadm upgrade node

sudo kubectl drain <node-to-drain> --ignore-daemonsets

sudo apt-mark unhold kubelet kubectl && \
sudo apt-get update && sudo apt-get install -y kubelet=1.27.x-00 kubectl=1.27.x-00 && \
sudo apt-mark hold kubelet kubectl

sudo systemctl daemon-reload
sudo systemctl restart kubelet

sudo kubectl uncordon <node-to-uncordon>

# on worker nodes

sudo apt-mark unhold kubeadm && \
sudo apt-get update && sudo apt-get install -y kubeadm=1.27.x-00 && \
sudo apt-mark hold kubeadm

sudo kubeadm upgrade node

sudo kubectl drain <node-to-drain> --ignore-daemonsets

sudo apt-mark unhold kubelet kubectl && \
sudo apt-get update && sudo apt-get install -y kubelet=1.27.x-00 kubectl=1.27.x-00 && \
sudo apt-mark hold kubelet kubectl

sudo systemctl daemon-reload
sudo systemctl restart kubelet

sudo kubectl uncordon <node-to-uncordon>


```

# VM 

```shell

# iso 

# processor 

# memory 

# storage 

# network: bridge  

```


# CLOUD (AWS)

```shell

# EC2: create EC2 instance  

# instance details: check instace details to get access 

# elastic block store: it has volume, instance info 

# network & security: check network & security  

# network interface: it has vpc, subnet, instance info 

# vpc: EC2 must be created in a same vpc 

# subnet: EC2 must be created in a same subnet under the same vpc 

# security group: each EC2 must share the same security(inbound, outbound) rule 

# instance: take care of firewall 

```

# CLOUD (GCP)

```shell

# api 

# oauth 
```

# AWS EKS

```shell
# eksctl version 0.106.0 

# create IAM user and role related to EKS and Adminaccess 

 

curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip" 

unzip awscliv2.zip 

sudo ./aws/install 

 

curl --silent --location "https://github.com/weaveworks/eksctl/releases/latest/download/eksctl_$(uname -s)_amd64.tar.gz" | tar xz -C /tmp 

 

mv /tmp/eksctl /usr/local/bin 

 

aws configure 

 

eksctl create cluster -f ekscluster.yaml 

 
```

# CUSTOM REGISTRY - HARBOR 

```shell

 

curl -s https://api.github.com/repos/goharbor/harbor/releases/latest | grep browser_download_url | cut -d '"' -f 4 | grep '\.tgz$' | wget -i - 

 

tar xvzf harbor-offline-installer*.tgz 

 

cp harbor.yml.tmpl harbor.yml 

 

# change necessary info 

 


```

```shell

# kubernetes private nodeport

helm repo add harbor https://helm.goharbor.io


helm install go-harbor harbor/harbor --set expose.type=nodePort --set expose.tls.auto.commonName=192.168.50.94 --set externalURL=https://192.168.50.94:30003 --set persistence.enabled=false --set imagePullPolicy=Always # --set persistence.enabled=true --set persistence.persistentVolumeClaim.registry.size=10Gi --set persistence.persistentVolumeClaim.chartmuseum.size=5Gi --set persistence.persistentVolumeClaim.jobservice.size=5Gi --set persistence.persistentVolumeClaim.database.size=5Gi --set persistence.persistentVolumeClaim.redis.size=5Gi


sudo kubectl exec go-harbor-nginx-9556767d6-tm89q -- cat /etc/nginx/cert/ca.crt > ca.crt 


mkdir -p /etc/docker/certs.d/192.168.50.94:30003

/bin/cp -Rf ca.crt /etc/docker/certs.d/192.168.50.94:30003/



```

# HELM

```shell

curl https://baltocdn.com/helm/signing.asc | sudo apt-key add -  

 

sudo apt install apt-transport-https --yes  

 

echo "deb https://baltocdn.com/helm/stable/debian/ all main" | sudo tee /etc/apt/sources.list.d/helm-stable-debian.list  

 

sudo apt update  

 

sudo apt install helm  
```

# JENKINS

```shell

sudo apt install -y openjdk-11-jdk openjdk-11-jre 

 

wget -q -O - https://pkg.jenkins.io/debian-stable/jenkins.io.key | sudo apt-key add - 

 

sudo sh -c 'echo deb http://pkg.jenkins.io/debian-stable binary/ > /etc/apt/sources.list.d/jenkins.list' 

 

sudo apt update 

 

sudo apt -y install jenkins 

 

# on k8s 

 

helm repo add jenkins https://charts.jenkins.io 

 

helm repo update 

 

helm install jkns jenkins/jenkins 

```


# PROMETHEUS & GRAFANA 

```shell

helm repo add prometheus-community https://prometheus-community.github.io/helm-charts  

  

helm repo add stable https://charts.helm.sh/stable  

 

helm repo update  

 

helm install kube-prometheus-stack prometheus-community/kube-prometheus-stack -f ./kube-prometheus-stack/value.yaml 



kubectl port-forward deployment/prometheus-grafana 3000 

 

# Prometheus alone 

 

helm repo add prometheus-community https://prometheus-community.github.io/helm-charts 

 

helm repo update 

 

helm install prometheus prometheus-community/prometheus 

 

# Grafana local 

 

sudo apt install -y apt-transport-https 

 

sudo apt install -y software-properties-common wget 

 

sudo wget -q -O /usr/share/keyrings/grafana.key https://packages.grafana.com/gpg.key 

 

echo "deb [signed-by=/usr/share/keyrings/grafana.key] https://packages.grafana.com/enterprise/deb stable main" | sudo tee -a /etc/apt/sources.list.d/grafana.list 

 

sudo apt update 

 

sudo apt install grafana-enterprise 

 

sudo service grafana-server start 

 

config -> /etc/grafana/grafana.ini 

 

# localhost:3000 

 

username: admin 

password: prom-operator 

 

```

# OCTANT

```shell

wget https://github.com/vmware-tanzu/octant/releases/download/v0.24.0/octant_0.24.0_Linux-64bit.deb 

 

dpkg -i octant_0.24.0_Linux-64bit.deb 


```


# REDIS

```yaml

version: "3"

services:

    redis:
      image: redis:7.0.8
      container_name: redis
      command:
        - /bin/sh
        - -c
        - redis-server --requirepass "youdonthavetoknow"
      volumes:
        - ./redis:/var/lib/redis/data
        - ./redis.conf:/usr/local/etc/redis/redis.conf
      ports:
        - "6379:6379"
```

```shell

# redis.conf

port              6379
daemonize         yes
save              60 1
bind              0.0.0.0
tcp-keepalive     300
dbfilename        dump.rdb
dir               ./
rdbcompression    yes

```

# KAFKA & ZOOKEEPER

```shell
helm repo add bitnami https://charts.bitnami.com/bitnami 

 

helm install zookeeper bitnami/zookeeper --set replicaCount=1 --set auth.enabled=false --set allowAnonymousLogin=true 

 

helm install kafka bitnami/kafka --set zookeeper.enabled=false --set replicaCount=1 --set externalZookeeper.servers=ZOOKEEPER-SERVICE-NAME 

 

kubectl --namespace default exec -it $POD_NAME -- kafka-console-consumer.sh --bootstrap-server KAFKA-SERVICE-NAME:9092 --topic mytopic 

 

kubectl --namespace default exec -it $POD_NAME -- kafka-console-producer.sh --broker-list KAFKA-SERVICE-NAME:9092 --topic mytopic 

 

# or kafkacat like below 

 

kafkacat -P -b KAFKA-SERVICE-NAME:9092 -t mytopic 

 

kafkacat -C -b KAFKA-SERVICE-NAME:9092 -t mytopic 

```
```yaml
# single node

---

version: "3"
services:
  zookeeper:
    image: 'bitnami/zookeeper:3.7'
    container_name: zookeeper
    ports:
      - '2181:2181'
    environment:
      - ALLOW_ANONYMOUS_LOGIN=yes
    volumes:
      - ./zook:/data
  kafka:
    image: 'bitnami/kafka:3.3'
    container_name: kafka
    ports:
      - '9092:9092'
    environment:
      - KAFKA_BROKER_ID=1
      - KAFKA_CFG_LISTENERS=PLAINTEXT://:9092
      - KAFKA_CFG_ADVERTISED_LISTENERS=PLAINTEXT://172.17.80.95:9092
      - KAFKA_CFG_ZOOKEEPER_CONNECT=zookeeper:2181
      - ALLOW_PLAINTEXT_LISTENER=yes
    volumes:
      - ./data:/var/lib/kafka/data
    depends_on:
      - zookeeper


```

# ELASTIC SEARCH & KIBANA 

```shell

helm repo add bitnami https://charts.bitnami.com/bitnami 

 

sudo helm install elasticsearch  bitnami/elasticsearch 

 

sudo helm install kibana bitnami/kibana --set elasticsearch.hosts[0]=elasticsearch --set elasticsearch.port=9200 

 

```

```yaml

version: "3"



services:

        elasticsearch:

                image: elasticsearch:8.6.1

                environment:

                        - xpack.security.enabled=true

                        - discovery.type=single-node

                        - "ES_JAVA_OPTS=-Xms2g -Xmx2g"

                        - ELASTIC_PASSWORD=youdonthavetoknow


                container_name: elasticsearch

                ports:

                        - "9200:9200"

                volumes:

                        - elastic_data:/usr/share/elasticsearch/data

volumes:

        elastic_data:

```
```shell
# set password for kibana

docker exec -it elasticsearch /bin/bash

bin/elasticsearch-set-password [ auto | interactive ]

```

```yaml

version: "3"
services:
    kibana:
        image: kibana:8.5.3
        container_name: kibana
        restart: always
        ports:
            - 5601:5601
        environment:
            - ELASTICSEARCH_USERNAME="kibana_system"
            - ELASTICSEARCH_PASSWORD="youdonthavetoknow"
            - ELASTICSEARCH_HOSTS=http://elasticsearch:9200
            - XPACK_MONITORING_ENABLED=true
            - XPACK_MONITORING_COLLECTION_ENABLED=true
            - XPACK_SECURITY_ENABLED=true
        networks:
            - network1

networks:
    network1:
        name: elastic_default
        external: true


```

# GPU

```shell

# nvidia cuda driver

# build prerequisite

sudo apt-get install linux-headers-$(uname -r) build-essential

# simple way

# curl from https://www.nvidia.com/download/index.aspx 

# and install run

# host driver 

# ubuntu 

sudo apt-get install linux-headers-$(uname -r) 

 

distribution=$(. /etc/os-release;echo $ID$VERSION_ID | sed -e 's/\.//g') 

 

wget https://developer.download.nvidia.com/compute/cuda/repos/$distribution/x86_64/cuda-keyring_1.0-1_all.deb 

 

sudo dpkg -i cuda-keyring_1.0-1_all.deb 

 

sudo apt-get update 

 

sudo apt-get -y install cuda-drivers 

 

#centos 

sudo dnf install -y tar bzip2 make automake gcc gcc-c++ pciutils elfutils-libelf-devel libglvnd-devel iptables firewalld vim bind-utils wget 

 

sudo yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm 

 

distribution=rhel7 

 

ARCH=$( /bin/arch ) 

 

sudo yum-config-manager --add-repo http://developer.download.nvidia.com/compute/cuda/repos/$distribution/${ARCH}/cuda-$distribution.repo 

 

sudo yum install -y kernel-devel-$(uname -r) kernel-headers-$(uname -r) 

 

sudo yum clean expire-cache 

 

sudo yum install -y nvidia-driver-latest-dkms 

 

# post install 

 

export PATH=/usr/local/cuda-11.8/bin${PATH:+:${PATH}} 

 

export LD_LIBRARY_PATH=/usr/local/cuda-11.8/lib64\ 

                         ${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}} 

 

sudo systemctl enable nvidia-persistenced 

 

 

/usr/bin/nvidia-persistenced --verbose 

 

cat /proc/driver/nvidia/version 

 
# disable noveau

sudo nano /etc/modprobe.d/blacklist-nouveau.conf

```
```shell
# /etc/modprobe.d/blacklist-nouveau.conf
blacklist nouveau
options nouveau modeset=0
```
```shell
sudo update-initramfs -u
sudo reboot
```

```shell

 

# install nvidia container toolkit 

 

sudo apt-get update && sudo apt-get install -y nvidia-container-toolkit 


```

```toml
# containerd daemon config /etc/containerd/config.toml


version = 2 

[plugins] 

  [plugins."io.containerd.grpc.v1.cri"] 

    [plugins."io.containerd.grpc.v1.cri".containerd] 

      default_runtime_name = "nvidia" 

  

      [plugins."io.containerd.grpc.v1.cri".containerd.runtimes] 

        [plugins."io.containerd.grpc.v1.cri".containerd.runtimes.nvidia] 

          privileged_without_host_devices = false 

          runtime_engine = "" 

          runtime_root = "" 

          runtime_type = "io.containerd.runc.v2" 

          [plugins."io.containerd.grpc.v1.cri".containerd.runtimes.nvidia.options] 

            BinaryName = "/usr/bin/nvidia-container-runtime" 

 
```

```shell
sudo systemctl restart containerd 

``` 

```shell
# crio daemon

# /etc/crio/crio.conf.d/crio.conf

[crio]

  [crio.runtime]
    default_runtime = "nvidia"

    [crio.runtime.runtimes]

      [crio.runtime.runtimes.nvidia]
        runtime_path = "/usr/bin/nvidia-container-runtime"
        runtime_type = "oci"

# or

# /etc/crio/crio.conf

 hooks_dir = [
        "/usr/share/containers/oci/hooks.d",
        "/run/containers/oci/hooks.d",
        "/etc/containers/oci/hooks.d",
 ]
 
 ```

```shell

$ sudo mkdir -p /usr/share/containers/oci/hooks.d 

# or

$ sudo mkdir -p /run/containers/oci/hooks.d

  

$ sudo bash -c  

cat > /usr/share/containers/oci/hooks.d/oci-nvidia-hook.json << EOF 

{ 

    "version": "1.0.0", 

    "hook": { 

        "path": "/usr/bin/nvidia-container-toolkit", 

        "args": ["nvidia-container-toolkit", "prestart"] ,
        
        "env": ["PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"]

    }, 

    "when": { 

        "always": true, 

        "commands": [".*"] 

    }, 

    "stages": ["prestart"] 

} 

EOF 



# or you could use simplified command

curl -fsSL https://nvidia.github.io/libnvidia-container/gpgkey | sudo gpg --dearmor -o /usr/share/keyrings/nvidia-container-toolkit-keyring.gpg \
  && curl -s -L https://nvidia.github.io/libnvidia-container/stable/deb/nvidia-container-toolkit.list | \
    sed 's#deb https://#deb [signed-by=/usr/share/keyrings/nvidia-container-toolkit-keyring.gpg] https://#g' | \
    sudo tee /etc/apt/sources.list.d/nvidia-container-toolkit.list \
  && \
    sudo apt-get update
    
    
sudo apt-get install -y nvidia-container-toolkit


sudo nvidia-ctk runtime configure --runtime=containerd

sudo systemctl restart docker


sudo nvidia-ctk runtime configure --runtime=containerd


sudo systemctl restart containerd

sudo nvidia-ctk runtime configure --runtime=crio


sudo systemctl restart crio
 

 

 

# docker 

 

sudo docker --gpus all 

# docker-compose

services:
  jupyterlabc:
    image: seantywork/jupyterlabc
    deploy:
      resources:
        reservations:
          devices:
            - driver: nvidia
              count: 1
              capabilities: [gpu] 

# kubernetes 

 

kubectl create -f https://raw.githubusercontent.com/NVIDIA/k8s-device-plugin/v0.13.0/nvidia-device-plugin.yml 

 

# helm 

 

helm repo add nvdp https://nvidia.github.io/k8s-device-plugin 

 

helm repo update 

 

helm upgrade -i nvdp nvdp/nvidia-device-plugin \ 

  --namespace nvidia-device-plugin \ 

  --create-namespace \ 

  --version 0.13.0 

 

 

```

```yaml
# gpu deployment 

apiVersion: apps/v1 

kind: Deployment 

metadata: 

  name: clusterdebuggergpu 

spec: 

  selector: 

    matchLabels: 

      app: clusterdebuggergpu 

  replicas: 1 

  template: 

    metadata: 

      labels: 

        app: clusterdebuggergpu 

    spec: 

      nodeSelector: 

        gpu-ready: ready 

      imagePullSecrets: 

        - name: harbor-secret 

      containers: 

        - name: clusterdebuggergpu 

          image: harbor.mipllab.com/library/clusterdebugger 

          resources: 

            limits: 

              nvidia.com/gpu: 1 

          imagePullPolicy: Always 

          ports: 

          - containerPort: 6700 



```

# GPU DRIVER UNINSTALL

```shell

sudo apt-get --purge remove "*nvidia*"

sudo /usr/bin/nvidia-uninstall

```

# GPU API

```shell

# nvidia cuda toolkit

# check nvidia-smi cuda version

# visit corresponding https://developer.nvidia.com/cuda-downloads 

wget https://developer.download.nvidia.com/compute/cuda/12.2.0/local_installers/cuda_12.2.0_535.54.03_linux.run

sudo sh cuda_12.2.0_535.54.03_linux.run

# add path and ld path

export PATH="/usr/local/cuda-12.2/bin:$PATH"

# /etc/ld.so.conf

...
include /usr/local/cuda-12.2/lib64
...

sudo ldconfig


# uninstall


/usr/local/cuda/bin/<uninstaller>


```

# LIB TORCH

```shell

# https://pytorch.org/get-started/locally/

# cpu

wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-2.1.1%2Bcpu.zip

unzip 

mv libtorch libtorch-cpu

# gpu

wget https://download.pytorch.org/libtorch/cu121/libtorch-cxx11-abi-shared-with-deps-2.1.1%2Bcu121.zip


# build 

mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/absolute/path/to/libtorch .. 
# cmake -DCMAKE_PREFIX_PATH=/home/seantywork/main/hack/0xalgo/libtorch .. (2.1)



cmake --build . --config Release

```

# VIRTUAL MACHINE BOX

```shell
# virtualbox

sudo apt update 

 

sudo apt install virtualbox 

 

 

 

sudo vboxmanage createvm --ostype Ubuntu_64 --basefolder "/srv/virtualbox" --register --name "ubuntu20" 

 

sudo vboxmanage modifyvm "ubuntu20" --cpus 2 --memory 2048 --nic1 bridged --bridgeadapter1 wlp2s0 

 

sudo vboxmanage createhd --filename "/srv/virtualbox/ubuntu20/ubuntu20.vdi" --format VDI --size 32000 

 

sudo vboxmanage storagectl "ubuntu20" --name "SATA" --add sata 

 

sudo vboxmanage storageattach "ubuntu20" --storagectl SATA --port 0 --type hdd --medium "/srv/virtualbox/ubuntu20/ubuntu20.vdi" 

 

sudo vboxmanage storagectl "ubuntu20" --name IDE --add ide 

 

sudo vboxmanage storageattach "ubuntu20" --storagectl IDE --port 0 --device 0 --type dvddrive --medium /home/seantywork/Downloads/ubuntu-20.04.5-live-server-amd64.iso  

 

 

sudo vboxmanage startvm "ubuntu20" --type headless 

 

sudo vboxmanage list runningvms 

 

sudo vboxmanage controlvm "ubuntu20" acpipowerbutton 

 
# clone 

 

sudo vboxmanage clonevm "ubuntu20" --name="ol7-dev-002" --register 




```

# VIRTUAL MACHINE QEMU

```shell
sudo apt install qemu-kvm virt-manager virtinst libvirt-clients bridge-utils libvirt-daemon-system -y

sudo systemctl enable --now libvirtd

sudo systemctl start libvirtd

sudo usermod -aG kvm $USER

sudo usermod -aG libvirt $USER

sudo virt-manager

# aarch64 on x64

# adding to the above first line

sudo apt-get install qemu-system-arm qemu-efi

# and set arch as aarch64 virt

# then set UEFI to aarch64


```

# VIRTUAL CLUSTER

```shell


# proxmox

create vm

import vm

import disk

qm importdisk <VM_ID> <OVA_DISK.vmdk> <VOL_NAME> -format qcow2

ex) qm importdisk 101 ubuntu20-disk001.vmdk local-lvm -format qcow2

create cluster

join cluster

```

# VM QEMU

```shell

sudo virsh shutdown <vm>

sudo virsh start <vm>

```

# VM QEMU NETWORK BRIDGE

```shell

# /etc/sysctl.d/10-bridge.conf

net.bridge.bridge-nf-call-ip6tables=0
net.bridge.bridge-nf-call-iptables=0
net.bridge.bridge-nf-call-arptables=0

```

```shell


echo "br_netfilter" > /etc/modules-load.d/br_netfilter.conf

reboot

virsh net-destroy default
virsh net-undefine default

# save /etc/netplan/01-network-manager-all.yaml

```

```yaml

# /etc/netplan/00-installer-config.yaml 

network:
  ethernets:
    enx44a92c521758:
      dhcp4: false
      dhcp6: false
  bridges:
    br0:
      interfaces: [ enx44a92c521758 ]
      addresses: [192.168.0.32/24]
      gateway4: 192.168.0.1
      mtu: 1500
      nameservers:
        addresses: [168.126.63.1,8.8.8.8]
      parameters:
        stp: true
        forward-delay: 4
      dhcp4: no
      dhcp6: no
  version: 2


```

```shell
sudo netplan apply


```

```shell
# host-bridge.xml

<network>
    <name>host-bridge</name>
    <bridge name='br0'/>
    <forward mode="bridge"/>
</network>

```

```shell

virsh net-define host-bridge.xml
virsh net-autostart host-bridge
virsh net-start host-bridge
virsh net-list --all

```

# VM QEMU GPU PASSTHROUGH

```shell

efibootmgr

# vt-x & vt-d enabled

# or amd-v & amd-iommu

# grub iommu config

/etc/default/grub
GRUB_CMDLINE_LINUX_DEFAULT="quiet  iommu=pt"

# or

GRUB_CMDLINE_LINUX_DEFAULT="quiet intel_iommu=on iommu=pt pcie_acs_override=downstream,multifunction nofb nomodeset initcall_blacklist=sysfb_init"

update-grub

reboot

dmesg | grep -e IOMMU

# vfio config

echo "vfio" >> /etc/modules
echo "vfio_iommu_type1" >> /etc/modules
echo "vfio_pci" >> /etc/modules

update-initramfs -u -k all

systemctl reboot

dmesg | grep -i vfio

dmesg | grep 'remapping'

# in case of no remapping

echo "options vfio_iommu_type1 allow_unsafe_interrupts=1" > /etc/modprobe.d/iommu_unsafe_interrupts.conf


# nvidia stability

echo "options kvm ignore_msrs=1 report_ignored_msrs=0" > /etc/modprobe.d/kvm.conf

# amd stability

apt install pve-headers-$(uname -r)
apt install git dkms build-essential
git clone https://github.com/gnif/vendor-reset.git
cd vendor-reset
dkms install .
echo "vendor-reset" >> /etc/modules
update-initramfs -u
shutdown -r now

lspci -nn | grep 'AMD'  # <DEVICE_ID> ex) 01:00.0

cat << EOF >>  /etc/systemd/system/vreset.service
[Unit]
Description=AMD GPU reset method to 'device_specific'
After=multi-user.target
[Service]
ExecStart=/usr/bin/bash -c 'echo device_specific > /sys/bus/pci/devices/0000:<DEVICE_ID>/reset_method'
[Install]
WantedBy=multi-user.target
EOF
systemctl enable vreset.service && systemctl start vreset.service


# gpu isolation and drivers

lspci -nn | grep 'NVIDIA' # or 'AMD'

echo "options vfio-pci ids=<ID>,<ID2>,..." > /etc/modprobe.d/vfio.conf

ex)

echo "options vfio-pci ids=1002:67df,1002:aaf0" > /etc/modprobe.d/vfio.conf


# AMD drivers
echo "blacklist radeon" >> /etc/modprobe.d/blacklist.conf
echo "blacklist amdgpu" >> /etc/modprobe.d/blacklist.conf
# NVIDIA drivers
echo "blacklist nouveau" >> /etc/modprobe.d/blacklist.conf
echo "blacklist nvidia" >> /etc/modprobe.d/blacklist.conf
echo "blacklist nvidiafb" >> /etc/modprobe.d/blacklist.conf
echo "blacklist nvidia_drm" >> /etc/modprobe.d/blacklist.conf
# Intel drivers
echo "snd_hda_intel" >> /etc/modprobe.d/blacklist.conf
echo "snd_hda_codec_hdmi" >> /etc/modprobe.d/blacklist.conf
echo "i915" >> /etc/modprobe.d/blacklist.conf


# gpu in vm

# bios ovmf (uefi)

# machine q35

# display vmware compatible

# boot vm without secure boot (esc at proxmox screen)

# check

lspci -nn

# noveau blacklist

sudo nano /etc/modprobe.d/blacklist-nouveau.conf

blacklist nouveau
options nouveau modeset=0

sudo update-initramfs -u

# turnoff

# add pci device without options

# turnon

# install corresponding gpu drivers

https://docs.nvidia.com/datacenter/tesla/tesla-installation-notes/index.html

# or 

https://www.nvidia.com/download/index.aspx

# gpu reset at reboot

```

```shell
# /root/reset_pci_gpu.sh
#!/bin/bash
echo 1 > /sys/bus/pci/devices/0000\:09\:00.0/remove
echo 1 > /sys/bus/pci/rescan

crontab -e

@reboot /root/reset_pci_gpu.sh

```


# IDA

```shell


# download run 

 

chmod  

 

sudo ./ida.run 

```
# GHIDRA

```shell

sudo apt-get install openjdk-17-jdk 

 

wget https://github.com/NationalSecurityAgency/ghidra/releases/download/Ghidra_10.2_build/ghidra_10.2_PUBLIC_20221101.zip 

 

unzip ghidra_10.2_PUBLIC_20221101.zip -d <target>
```

# WIRESHARK

```shell

sudo apt install wireshark 


```

# TCP DUMP TCPDUMP

```shell

sudo tcpdump -i $IFACE -A -X -c 20 port $PORT
```

# TOR

```shell

# download tar or 

 

wget https://www.torproject.org/dist/torbrowser/11.5.7/tor-browser-linux64-11.5.7_en-US.tar.xz 

 

tar -xvf tor-browser-linux64-11.5.7_en-US.tar.xz 

 
# install command line

apt install apt-transport-https

```


```shell

# /etc/apt/sources.list.d/tor.list

   deb     [arch=amd64 signed-by=/usr/share/keyrings/tor-archive-keyring.gpg] https://deb.torproject.org/torproject.org jammy main
   deb-src [arch=amd64 signed-by=/usr/share/keyrings/tor-archive-keyring.gpg] https://deb.torproject.org/torproject.org jammy main

```

```shell

wget -qO- https://deb.torproject.org/torproject.org/A3C4F0F979CAA22CDBA8F512EE8CBC9E886DDD89.asc | gpg --dearmor | sudo tee /usr/share/keyrings/tor-archive-keyring.gpg >/dev/null


apt update

apt install tor deb.torproject.org-keyring

```

# METASPLOIT AND SECURITY TOOLS 

```shell


curl https://raw.githubusercontent.com/rapid7/metasploit-omnibus/master/config/templates/metasploit-framework-wrappers/msfupdate.erb > msfinstall 

 

sudo chmod 755 msfinstall 

 

sudo ./msfinstall 

 

sudo apt install nmap 

 

git clone --depth 1 https://github.com/sqlmapproject/sqlmap.git sqlmap-dev 

 

sudo apt install nikto -y 

 
```

# NGROK

```shell

sudo snap install ngrok

ngrok tcp <port>
```

# BURPESUITE

```shell


# download .sh 

 

sudo ./burpsuite_community_linux_v2022_9_5.sh  

 

```

# OWASP ZAP

```shell

# download .zip 

 

tar -xzf ZAP_2.12.0_Linux.tar.gz 


```

# OLD KEY STORE FIX

```shell

for KEY in $(apt-key --keyring /etc/apt/trusted.gpg list | grep -E "(([ ]{1,2}(([0-9A-F]{4}))){10})" | tr -d " " | grep -E "([0-9A-F]){8}\b" ); do K=${KEY:(-8)}; apt-key export $K | sudo gpg --dearmour -o /etc/apt/trusted.gpg.d/imported-from-trusted-gpg-$K.gpg; done
 

 

```

# CLUSTER DEBUGGER

```yaml

apiVersion: apps/v1
kind: Deployment
metadata:
  name: clusterdebugger
spec:
  selector:
    matchLabels:
      app: clusterdebugger
  replicas: 1
  template:
    metadata:
      labels:
        app: clusterdebugger
    spec:
      containers:
        - name: clusterdebugger
          image: seantywork/clusterdebugger
          imagePullPolicy: Always
          ports:
          - containerPort: 6700
```


```yaml

version: "3"


services:

        clusterdebugger:

                build: .


                container_name: clusterdebugger

                ports:

                        - "6700:6700"

```

```shell
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y ca-certificates curl gnupg lsb-release wget iptables supervisor git vim

RUN apt install -y python3-pip

RUN pip3 install gunicorn

RUN pip3 install flask

RUN pip3 install requests

RUN pip3 install Flask-Session

RUN pip3 install mysql-connector-python

CMD ["tail", "-f", "/dev/null"]

```

# CONTAINER DEBUGGER

```shell

FROM ubuntu:20.04


RUN apt-get update

RUN apt-get install -y build-essential ca-certificates curl git vim

RUN mkdir -p /workspace

WORKDIR /workspace

CMD ["tail","-f","/dev/null"]
```


```yaml
version: "3"

services:

    debugger:
      build: .
      container_name: debugger


```

# ELASTIC SEARCH

```shell

FROM elasticsearch:8.6.1

ENV xpack.security.enabled=true
ENV discovery.type=single-node
ENV ES_JAVA_OPTS="-Xms2g -Xmx2g"
ENV ELASTIC_PASSWORD=estest

EXPOSE 9200

```

```yaml


version: "3"


services:

        elasticsearch:

                build: ./elasticsearch


                container_name: elasticsearch

                ports:

                        - "9200:9200"

                volumes:

                        - ./elasticsearch/data:/usr/share/elasticsearch/data

```

```yaml
apiVersion: v1
kind: Service
metadata:
  name: elasticsearch
  labels:
    app: elasticsearch
spec:
  type: ClusterIP
  ports:
  - port: 9200
    targetPort: 9200
    protocol: TCP
  selector:
    app: elasticsearch
---
apiVersion: apps/v1
kind: Deployment
metadata:
  name: elasticsearch
spec:
  selector:
    matchLabels:
      app: elasticsearch
  replicas: 1
  template:
    metadata:
      labels:
        app: elasticsearch
    spec:
      volumes:
        - name: elasticsearch-storage
          persistentVolumeClaim:
            claimName: pvc-16g-elasticsearch
      imagePullSecrets:
        - name: harbor-secret
      containers:
        - name: elasticsearch
          image: seantywork/elasticsearch
          imagePullPolicy: Always
          ports:
          - containerPort: 9200
          volumeMounts:
          - mountPath: /usr/share/elasticsearch/data
            name: elasticsearch-storage
---
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: pvc-16g-elasticsearch
spec:
  storageClassName: nfs-default-storageclass
  accessModes:
    - ReadWriteOnce
  resources:
    requests:
      storage: 16Gi

```

```yaml
apiVersion: v1
kind: PersistentVolume
metadata:
  name: pv-16g-elasticsearch
spec:
  capacity:
    storage: 16Gi
  volumeMode: Filesystem
  accessModes:
    - ReadWriteOnce
  persistentVolumeReclaimPolicy: Delete
  storageClassName: nfs-default-storageclass
  mountOptions:
    - hard
    - nfsvers=4.1
  nfs:
    path: /data1/nfs-cluster-vol/es-kbn/elastic/elasticsearch/data
    server: 192.168.0.100


```

# GITLAB

```yaml

version: "3"
services:
  gitlab:
    image: gitlab/gitlab-ce:15.4.6-ce.0
    restart: always
    container_name: gitlab
    hostname: 'gitlab.usagecorpus.com'
    environment:
      GITLAB_OMNIBUS_CONFIG: |
        external_url 'http://gitlab.usagecorpus.com'
    ports:
      - '9367:80'
    volumes:
      - './gitlab-data/config:/etc/gitlab'
      - './gitlab-data/logs:/var/log/gitlab'
      - './gitlab-data/data:/var/opt/gitlab'
    shm_size: '256m'

```


# GPU SET

```shell

#!/bin/bash


# Adjust versions, or, you might even have to directly download driver installtion script 

sudo apt-get install linux-headers-$(uname -r)



distribution=$(. /etc/os-release;echo $ID$VERSION_ID | sed -e 's/\.//g')



wget https://developer.download.nvidia.com/compute/cuda/repos/$distribution/x86_64/cuda-keyring_1.0-1_all.deb



sudo dpkg -i cuda-keyring_1.0-1_all.deb



sudo apt-get update



sudo apt-get -y install cuda-drivers



export PATH=/usr/local/cuda-11.8/bin${PATH:+:${PATH}}



export LD_LIBRARY_PATH=/usr/local/cuda-11.8/lib64\

                         ${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}



sudo systemctl enable nvidia-persistenced



/usr/bin/nvidia-persistenced --verbose



sudo apt-get update && sudo apt-get install -y nvidia-container-toolkit



echo "Now configure container runtime"
echo "Go to the link below to get detailed guidance"
echo "https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html"

```

# HARBOR

```shell

# get the zip? tar?

# untar

# edit harbor.yml

```

```yaml
# harbor.yml

...

hostname: harbor.usagecorpus.com

# http related config
http:
  # port for http, default is 80. If https enabled, this port will redirect to https port
  port: 5001

# https related config
#https:
#  # https port for harbor, default is 443
#  port: 443
#  # The path of cert and key files for nginx
#  certificate: /your/certificate/path
#  private_key: /your/private/key/path

# # Uncomment following will enable tls communication between all harbor components
# internal_tls:
#   # set enabled to true means internal tls is enabled
#   enabled: true
#   # put your cert and key files on dir
#   dir: /etc/harbor/tls/internal

# Uncomment external_url if you want to enable external proxy
# And when it enabled the hostname will no longer used
external_url: https://harbor.usagecorpus.com

# The initial password of Harbor admin
# It only works in first time to install harbor
# Remember Change the admin password from UI after launching Harbor.
harbor_admin_password: Harbor12345

# Harbor DB configuration
database:
  # The password for the root user of Harbor DB. Change this before any production use.
  password: root123
  # The maximum number of connections in the idle connection pool. If it <=0, no idle connections are retained.
  max_idle_conns: 100
  # The maximum number of open connections to the database. If it <= 0, then there is no limit on the number of open connections.
  # Note: the default number of connections is 1024 for postgres of harbor.
  max_open_conns: 900
  # The maximum amount of time a connection may be reused. Expired connections may be closed lazily before reuse. If it <= 0, connections are not closed due to a connection's age.
  # The value is a duration string. A duration string is a possibly signed sequence of decimal numbers, each with optional fraction and a unit suffix, such as "300ms", "-1.5h" or "2h45m". Valid time units are "ns", "us" (or "µs"), "ms", "s", "m", "h".
  conn_max_lifetime: 5m
  # The maximum amount of time a connection may be idle. Expired connections may be closed lazily before reuse. If it <= 0, connections are not closed due to a connection's idle time.
  # The value is a duration string. A duration string is a possibly signed sequence of decimal numbers, each with optional fraction and a unit suffix, such as "300ms", "-1.5h" or "2h45m". Valid time units are "ns", "us" (or "µs"), "ms", "s", "m", "h".
  conn_max_idle_time: 0

# The default data volume
data_volume: /data2/harbor/harbor_data

...


```


# JENKINS CONTAINER

```shell
FROM ubuntu:20.04

RUN apt update

RUN apt install -y openjdk-11-jre openjdk-11-jdk wget gnupg2 curl

RUN wget -q -O - https://pkg.jenkins.io/debian-stable/jenkins.io.key | apt-key add -

RUN sh -c 'echo deb http://pkg.jenkins.io/debian-stable binary/ > /etc/apt/sources.list.d/jenkins.list'

RUN apt update

RUN apt install -y jenkins


RUN curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose

RUN chmod +x /usr/local/bin/docker-compose

RUN apt update

RUN apt install -y apt-transport-https ca-certificates curl

RUN curl -fsSLo /usr/share/keyrings/kubernetes-archive-keyring.gpg https://packages.cloud.google.com/apt/doc/apt-key.gpg

RUN echo "deb [signed-by=/usr/share/keyrings/kubernetes-archive-keyring.gpg] https://apt.kubernetes.io/ kubernetes-xenial main" | tee /etc/apt/sources.list.d/kubernetes.list

RUN apt update

RUN apt install -y ca-certificates curl gnupg lsb-release

RUN mkdir -p /etc/apt/keyrings

RUN curl -fsSL https://download.docker.com/linux/ubuntu/gpg | gpg --dearmor -o /etc/apt/keyrings/docker.gpg

RUN echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" | tee /etc/apt/sources.list.d/docker.list > /dev/null

RUN apt update

RUN apt install -y docker-ce docker-ce-cli containerd.io docker-compose-plugin

RUN mkdir /root/.ssh


EXPOSE 8080

VOLUME /var/run/docker.sock
VOLUME /root/.jenkins
VOLUME /root/.ssh

WORKDIR /home

CMD ["jenkins"]

```

```yaml

version: "3"

services:
        jenkins:
                build: ./JK
                container_name: jenkins
                ports:

                        - "8080:8080"
             


                volumes:
                        - /var/run/docker.sock:/var/run/docker.sock
                        - ./JK/jenkins_data:/root/.jenkins
                        - /home/tomcat/.ssh:/root/.ssh


```


# JUPYTER LAB

```yaml
version: "3"

services:

        jupyter:
                build: ./build_jupyter
                container_name: jupyter
                ports:
                        - "8888:8888"
                volumes:
                        - ./jupyter:/root


```

```shell
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update

RUN apt install -y git

RUN apt install -y vim

RUN apt install -y curl

RUN apt install -y gnupg

RUN apt install -y python3-pip

RUN pip3 install jupyterlab==3.5.1
RUN pip3 install gekko==1.0.5
RUN pip3 install beautifulsoup4==4.11.2
RUN pip3 install keras==2.11.0
RUN pip3 install matplotlib==3.6.2
RUN pip3 install numpy==1.23.5
RUN pip3 install opencv-python==4.6.0.66
RUN pip3 install pandas==1.5.2
RUN pip3 install plotly==5.11.0
RUN pip3 install torch==1.13.1
RUN pip3 install torchvision==0.14.1
RUN pip3 install scikit-learn==1.1.3
RUN pip3 install scipy==1.9.3
RUN pip3 install seaborn==0.12.1
RUN pip3 install statsmodels==0.13.5
RUN pip3 install tensorflow==2.11.0
RUN pip3 install xgboost==1.7.1
RUN pip3 install requests==2.28.1
RUN pip3 install urllib3==1.26.13
RUN pip3 install selenium==4.7.2
RUN pip3 install mysql-connector-python==8.0.31
RUN pip3 install elasticsearch==8.5.2
RUN pip3 install kafka-python==2.0.2


CMD ["jupyter","lab","--ip","0.0.0.0","--no-browser","--port=8888","--allow-root"]


```

```yaml

apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: ingress-jupyter
  annotations:
      nginx.ingress.kubernetes.io/proxy-body-size: "0"

spec:
  rules:
  - host: jupyter.usagecorpus.com
    http:
      paths:
        - path: /
          pathType: Prefix
          backend:
            service:
              name: jupyter
              port:
                number: 8888
```


```yaml

apiVersion: v1
kind: Service
metadata:
  name: jupyter
  labels:
    app: jupyter
spec:
  type: ClusterIP
  ports:
  - port: 8888
    targetPort: 8888
    protocol: TCP
  selector:
    app: jupyter
---
apiVersion: apps/v1
kind: Deployment
metadata:
  name: jupyter
spec:
  selector:
    matchLabels:
      app: jupyter
  replicas: 1
  template:
    metadata:
      labels:
        app: jupyter
    spec:
      nodeSelector:
        gpu-ready1: ready
      volumes:
        - name: jupyter-storage-jupyter
          persistentVolumeClaim:
            claimName: pvc-8g-jupyter-jupyter
      imagePullSecrets:
      - name: harbor-secret
      containers:
        - name: jupyter
          image: seantywork/jupyter
          resources:
            limits:
              nvidia.com/gpu: 1
          imagePullPolicy: Always
          ports:
          - containerPort: 8888
          volumeMounts:
          - mountPath: /root
            name: jupyter-storage-jupyter
```


```yaml
apiVersion: v1
kind: PersistentVolume
metadata:
  name: pv-8g-jupyter-jupyter
spec:
  capacity:
    storage: 8Gi
  volumeMode: Filesystem
  accessModes:
    - ReadWriteOnce
  persistentVolumeReclaimPolicy: Delete
  storageClassName: nfs-default-storageclass
  mountOptions:
    - hard
    - nfsvers=4.1
  nfs:
    path: /data1/nfs-cluster-vol/jupyter/jupyter
    server: 192.168.0.100
---
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: pvc-8g-jupyter-jupyter
spec:
  storageClassName: nfs-default-storageclass
  accessModes:
    - ReadWriteOnce
  resources:
    requests:
      storage: 8Gi

```


```yaml
apiVersion: v1
kind: PersistentVolume
metadata:
  name: pv-16g-jupyter-mldldb
spec:
  capacity:
    storage: 16Gi
  volumeMode: Filesystem
  accessModes:
    - ReadWriteOnce
  persistentVolumeReclaimPolicy: Delete
  storageClassName: nfs-default-storageclass
  mountOptions:
    - hard
    - nfsvers=4.1
  nfs:
    path: /data1/nfs-cluster-vol/jupyter/mldldb
    server: 192.168.0.100
---
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: pvc-16g-jupyter-mldldb
spec:
  storageClassName: nfs-default-storageclass
  accessModes:
    - ReadWriteOnce
  resources:
    requests:
      storage: 16Gi

```

# KUBERNETES K8S


```shell
#!/bin/bash

set -euxo pipefail


######## wj


HOME="/root" 


sudo swapoff -a


(crontab -l 2>/dev/null; echo "@reboot /sbin/swapoff -a") | crontab - || true
sudo apt-get update -y



#OS="xUbuntu_20.04"

OS="$2"

#VERSION="1.25"

VERSION="$3"

cat <<EOF | sudo tee /etc/modules-load.d/crio.conf
overlay
br_netfilter
EOF

sudo modprobe overlay
sudo modprobe br_netfilter


cat <<EOF | sudo tee /etc/sysctl.d/99-kubernetes-cri.conf
net.bridge.bridge-nf-call-iptables  = 1
net.ipv4.ip_forward                 = 1
net.bridge.bridge-nf-call-ip6tables = 1
EOF

sudo sysctl --system


cat <<EOF | sudo tee /etc/apt/sources.list.d/devel:kubic:libcontainers:stable.list
deb https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/$OS/ /
EOF
cat <<EOF | sudo tee /etc/apt/sources.list.d/devel:kubic:libcontainers:stable:cri-o:$VERSION.list
deb http://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable:/cri-o:/$VERSION/$OS/ /
EOF

curl -L https://download.opensuse.org/repositories/devel:kubic:libcontainers:stable:cri-o:$VERSION/$OS/Release.key | sudo apt-key --keyring /etc/apt/trusted.gpg.d/libcontainers.gpg add -
curl -L https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/$OS/Release.key | sudo apt-key --keyring /etc/apt/trusted.gpg.d/libcontainers.gpg add -

sudo apt-get update
sudo apt-get install cri-o cri-o-runc -y

sudo systemctl daemon-reload
sudo systemctl enable crio --now

echo "Container runtime installed susccessfully"

sudo apt-get update
sudo apt-get install -y apt-transport-https ca-certificates curl

sudo mkdir -p /etc/apt/keyrings


sudo curl -fsSL "https://pkgs.k8s.io/core:/stable:/v$VERSION/deb/Release.key" | sudo gpg --dearmor -o /etc/apt/keyrings/kubernetes-apt-keyring.gpg

echo "deb [signed-by=/etc/apt/keyrings/kubernetes-apt-keyring.gpg] https://pkgs.k8s.io/core:/stable:/v$VERSION/deb/ /" | sudo tee /etc/apt/sources.list.d/kubernetes.list

sudo apt-get update -y
sudo apt-get install -y kubelet kubectl kubeadm
sudo apt-get update -y
sudo apt-get install -y jq



local_ip=$1
cat > /etc/default/kubelet << EOF
KUBELET_EXTRA_ARGS=--node-ip=$local_ip
EOF


MASTER_IP=$local_ip
LB_IP=$local_ip
IP_NO_DOT=$(echo "$local_ip" | sed "s/\./-/g")
PORT="3446"
NODENAME="$(hostname -s)"
NODENAME="lead-$NODENAME-$IP_NO_DOT-$(openssl rand -hex 8)"
POD_CIDR="10.10.0.0/16"

sudo kubeadm config images pull 

echo "Preflight Check Passed: Downloaded All Required Images"

sudo kubeadm init --apiserver-advertise-address=$MASTER_IP --apiserver-cert-extra-sans="$MASTER_IP,$LB_IP" --pod-network-cidr=$POD_CIDR --node-name "$NODENAME" --control-plane-endpoint "$LB_IP:$PORT" --ignore-preflight-errors Swap 
mkdir -p "$HOME"/.kube
sudo cp -i /etc/kubernetes/admin.conf "$HOME"/.kube/config
sudo chown "$(id -u)":"$(id -g)" "$HOME"/.kube/config



kubectl apply -f https://raw.githubusercontent.com/projectcalico/calico/v3.25.0/manifests/calico.yaml

kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml



kubectl label node $NODENAME ingress-ready=true 

```


```shell

#!/bin/bash


set -euxo pipefail



HOME="/root" 

sudo swapoff -a


(crontab -l 2>/dev/null; echo "@reboot /sbin/swapoff -a") | crontab - || true
sudo apt-get update -y



OS="$2"

VERSION="$3"


cat <<EOF | sudo tee /etc/modules-load.d/crio.conf
overlay
br_netfilter
EOF

sudo modprobe overlay
sudo modprobe br_netfilter


cat <<EOF | sudo tee /etc/sysctl.d/99-kubernetes-cri.conf
net.bridge.bridge-nf-call-iptables  = 1
net.ipv4.ip_forward                 = 1
net.bridge.bridge-nf-call-ip6tables = 1
EOF

sudo sysctl --system


cat <<EOF | sudo tee /etc/apt/sources.list.d/devel:kubic:libcontainers:stable.list
deb https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/$OS/ /
EOF
cat <<EOF | sudo tee /etc/apt/sources.list.d/devel:kubic:libcontainers:stable:cri-o:$VERSION.list
deb http://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable:/cri-o:/$VERSION/$OS/ /
EOF

curl -L https://download.opensuse.org/repositories/devel:kubic:libcontainers:stable:cri-o:$VERSION/$OS/Release.key | sudo apt-key --keyring /etc/apt/trusted.gpg.d/libcontainers.gpg add -
curl -L https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/$OS/Release.key | sudo apt-key --keyring /etc/apt/trusted.gpg.d/libcontainers.gpg add -

sudo apt-get update
sudo apt-get install cri-o cri-o-runc -y

sudo systemctl daemon-reload
sudo systemctl enable crio --now

echo "Container runtime installed susccessfully"


sudo apt-get update

sudo apt-get install -y apt-transport-https ca-certificates curl

sudo mkdir -p /etc/apt/keyrings

sudo curl -fsSL "https://pkgs.k8s.io/core:/stable:/v$VERSION/deb/Release.key" | sudo gpg --dearmor -o /etc/apt/keyrings/kubernetes-apt-keyring.gpg

echo "deb [signed-by=/etc/apt/keyrings/kubernetes-apt-keyring.gpg] https://pkgs.k8s.io/core:/stable:/v$VERSION/deb/ /" | sudo tee /etc/apt/sources.list.d/kubernetes.list

sudo apt-get update -y
sudo apt-get install -y kubelet kubectl kubeadm
sudo apt-get update -y
sudo apt-get install -y jq



local_ip=$1
cat > /etc/default/kubelet << EOF
KUBELET_EXTRA_ARGS=--node-ip=$local_ip
EOF
```

```shell

#!/bin/bash

sudo apt-get update

sudo apt-get install -y  haproxy 

```

```shell
#!/bin/bash

helm repo add prometheus-community https://prometheus-community.github.io/helm-charts

helm repo update

cd ./prom-grafana-charts/charts

helm install kube-prometheus-stack prometheus-community/kube-prometheus-stack -f ./default-kube-prom-custom-value.yaml --version 42.2.0


```


```shell
#!/bin/bash

helm repo add nfs-subdir-external-provisioner https://kubernetes-sigs.github.io/nfs-subdir-external-provisioner/


helm install nfs-subdir-external-provisioner nfs-subdir-external-provisioner/nfs-subdir-external-provisioner \
    --set nfs.server=192.168.0.25 \
    --set nfs.path=/data


```

```shell
#!/bin/bash

helm repo add nvdp https://nvidia.github.io/k8s-device-plugin



helm repo update



helm upgrade -i nvdp nvdp/nvidia-device-plugin \

  --namespace nvidia-device-plugin \

  --create-namespace \

  --version 0.13.0





```

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: ingress-grafana

spec:
  rules:
  - host: grafana.mipllab.com
    http:
      paths:
        - path: /
          pathType: Prefix
          backend:
            service:
              name: kube-prometheus-stack-grafana
              port:
                number: 80

```


```shell
...

#---------------------------------------------------------------------
# main frontend which proxys to the backends
#---------------------------------------------------------------------
frontend kubernetes
    bind *:3446
    option tcplog
    mode tcp
    default_backend kube_master_nodes    


#---------------------------------------------------------------------
# static backend for serving up images, stylesheets and such
#---------------------------------------------------------------------
#backend static
#    balance     roundrobin
#    server      static 127.0.0.1:4331 check

#---------------------------------------------------------------------
# round robin balancing between the various backends
#---------------------------------------------------------------------
backend kube_master_nodes
    mode tcp
    balance     roundrobin
    option tcp-check
    server  -> check fall 3 rise 2



...




```


```shell

#!/bin/bash


ufw allow from 192.168.0.1/24 

ufw allow from 10.10.0.0/16



```


```shell

#!/bin/bash

firewall-cmd --permanent --add-port=3446/tcp

firewall-cmd --permanent --add-port=6443/tcp

firewall-cmd --permanent --add-port=2379-2380/tcp

firewall-cmd --permanent --add-port=10250/tcp

firewall-cmd --permanent --add-port=10259/tcp

firewall-cmd --permanent --add-port=10257/tcp

firewall-cmd --permanent --add-port=10250/tcp

firewall-cmd --permanent --add-port=30000-32767/tcp

firewall-cmd --permanent --add-port=80/tcp

firewall-cmd --permanent --add-port=443/tcp

firewall-cmd --zone=trusted --add-source=192.168.0.1/24 --permanent

firewall-cmd --zone=trusted --add-source=10.10.0.0/16 --permanent

```


```yaml

kind: StorageClass
apiVersion: storage.k8s.io/v1
metadata:
  annotations:
    storageclass.kubernetes.io/is-default-class: "true"
  name: nfs-default-storageclass
provisioner: cluster.local/nfs-subdir-external-provisioner
reclaimPolicy: Delete
allowVolumeExpansion: true
volumeBindingMode: Immediate

```

```yaml

alertmanager:
  alertmanagerSpec:
    storage:
     volumeClaimTemplate:
       spec:
         storageClassName: nfs-default-storageclass
         accessModes: ["ReadWriteOnce"]
         resources:
           requests:
             storage: 4Gi
grafana:
  persistence:
    enabled: true
    type: pvc
    storageClassName: nfs-default-storageclass
    accessModes:
    - ReadWriteOnce
    size: 4Gi
prometheus:
  prometheusSpec:
    storageSpec:
      volumeClaimTemplate:
        spec:
          storageClassName: nfs-default-storageclass
          accessModes: ["ReadWriteOnce"]
          resources:
            requests:
              storage: 4Gi
```

```yaml
apiVersion: v1
kind: Service
metadata:
  name: clusterdebugger
  labels:
    app: clusterdebugger
spec:
  type: ClusterIP
  ports:
  - port: 6700
    targetPort: 6700
    protocol: TCP
  selector:
    app: clusterdebugger
---
apiVersion: apps/v1
kind: Deployment
metadata:
  name: clusterdebugger
spec:
  selector:
    matchLabels:
      app: clusterdebugger
  replicas: 1
  template:
    metadata:
      labels:
        app: clusterdebugger
    spec:
      imagePullSecrets:
        - name: harbor-secret
      containers:
        - name: clusterdebugger
          image: harbor.mipllab.com/library/clusterdebugger
          imagePullPolicy: Always
          ports:
          - containerPort: 6700


```

```yaml
apiVersion: v1
kind: Service
metadata:
  name: clusterdebuggergpu
  labels:
    app: clusterdebuggergpu
spec:
  type: ClusterIP
  ports:
  - port: 6700
    targetPort: 6700
    protocol: TCP
  selector:
    app: clusterdebuggergpu
---
apiVersion: apps/v1
kind: Deployment
metadata:
  name: clusterdebuggergpu
spec:
  selector:
    matchLabels:
      app: clusterdebuggergpu
  replicas: 1
  template:
    metadata:
      labels:
        app: clusterdebuggergpu
    spec:
      nodeSelector:
        gpu-ready1: ready
      imagePullSecrets:
        - name: harbor-secret
      containers:
        - name: clusterdebuggergpu
          image: harbor.mipllab.com/library/clusterdebugger
          resources:
            limits:
              nvidia.com/gpu: 1
          imagePullPolicy: Always
          ports:
          - containerPort: 6700


```


```shell
#!/bin/bash


USER=tomcat # customizable
mkdir -p /etc/kubernetes/pki/etcd
mv /home/${USER}/ca.crt /etc/kubernetes/pki/
mv /home/${USER}/ca.key /etc/kubernetes/pki/
mv /home/${USER}/sa.pub /etc/kubernetes/pki/
mv /home/${USER}/sa.key /etc/kubernetes/pki/
mv /home/${USER}/front-proxy-ca.crt /etc/kubernetes/pki/
mv /home/${USER}/front-proxy-ca.key /etc/kubernetes/pki/
mv /home/${USER}/etcd-ca.crt /etc/kubernetes/pki/etcd/ca.crt
mv /home/${USER}/etcd-ca.key /etc/kubernetes/pki/etcd/ca.key
mv /home/${USER}/admin.conf /etc/kubernetes/admin.conf



```

```shell
#!/bin/bash

USER=tomcat # customizable
# Set the control_plane_ips to all other master node ips or hostnames
CONTROL_PLANE_IPS="192.168.0.102 192.168.0.103"
for host in ${CONTROL_PLANE_IPS}; do
    scp /etc/kubernetes/pki/ca.crt "${USER}"@$host:
    scp /etc/kubernetes/pki/ca.key "${USER}"@$host:
    scp /etc/kubernetes/pki/sa.key "${USER}"@$host:
    scp /etc/kubernetes/pki/sa.pub "${USER}"@$host:
    scp /etc/kubernetes/pki/front-proxy-ca.crt "${USER}"@$host:
    scp /etc/kubernetes/pki/front-proxy-ca.key "${USER}"@$host:
    scp /etc/kubernetes/pki/etcd/ca.crt "${USER}"@$host:etcd-ca.crt
    scp /etc/kubernetes/pki/etcd/ca.key "${USER}"@$host:etcd-ca.key
    scp /etc/kubernetes/admin.conf "${USER}"@$host:
done



```

# KAFKA

```shell
#!/bin/bash



helm repo add bitnami https://charts.bitnami.com/bitnami




helm install zookeeper bitnami/zookeeper --set replicaCount=1 --set auth.enabled=false --set allowAnonymousLogin=true



helm install kafka bitnami/kafka --set zookeeper.enabled=false --set replicaCount=1 --set externalZookeeper.servers=ZOOKEEPER-SERVICE-NAME


```
# KIBANA


```yaml

apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: ingress-kibana
  annotations:
      nginx.ingress.kubernetes.io/proxy-body-size: "0"

spec:
  rules:
  - host: kibana.usagecorpus.com
    http:
      paths:
        - path: /
          pathType: Prefix
          backend:
            service:
              name: kibana
              port:
                number: 5601



```

```yaml

apiVersion: v1
kind: Service
metadata:
  name: kibana
  labels:
    app: kibana
spec:
  type: ClusterIP
  ports:
  - port: 5601
    targetPort: 5601
    protocol: TCP
  selector:
    app: kibana
---
apiVersion: apps/v1
kind: Deployment
metadata:
  name: kibana
spec:
  selector:
    matchLabels:
      app: kibana
  replicas: 1
  template:
    metadata:
      labels:
        app: kibana
    spec:
      imagePullSecrets:
        - name: harbor-secret
      containers:
        - name: kibana
          image: seantywork/kibana
          imagePullPolicy: Always
          ports:
          - containerPort: 5601



```


```yaml

version: "3"
services:
    kibana:
        build: ./kibana
        container_name: kibana
        restart: always
        ports:
            - 5601:5601
        networks:
            - network1

networks:
    network1:
        name: elastic_default
        external: true
```

```shell
FROM kibana:8.5.3

ENV ELASTICSEARCH_USERNAME="kibana_system"
ENV ELASTICSEARCH_PASSWORD="estest"
ENV ELASTICSEARCH_HOSTS=http://elasticsearch:9200
ENV XPACK_MONITORING_ENABLED=true
ENV XPACK_MONITORING_COLLECTION_ENABLED=true
ENV XPACK_SECURITY_ENABLED=true

EXPOSE 5601


```

# MONGO DB

```yaml
version: "3"
services:
        mongodb:
                build: ./mdb
                container_name: mongodb
                ports:
                        - 27017:27017
                volumes:
                        - ./mdb/data:/data/db


```


```shell
FROM mongo:4.4.19-rc1

ENV MONGO_INITDB_ROOT_USERNAME root
ENV MONGO_INITDB_ROOT_PASSWORD youdonthavetoknow

EXPOSE 27017

```

# NFS

```shell

#!/bin/bash

sudo dnf install nfs-utils



sudo systemctl start nfs-server.service


sudo systemctl enable --now nfs-server.service


## /etc/exports

## exportfs -a


```

```shell
#!/bin/bash

apt update

apt install -y nfs-kernel-server

apt install -y nfs-common

apt install -y jq

mkdir -p /kw_data

chown -R root:root /kw_data

chmod -R 777 /kw_data



local_ip=$1

IP=$local_ip

cat << EOF > /etc/exports

/kw_data $IP/24(rw,sync,no_subtree_check,no_root_squash) 10.10.0.0/16(rw,sync,no_subtree_check,no_root_squash)

EOF

exportfs -a

systemctl restart nfs-kernel-server

```


# NGINX CONF

```shell

user www-data;
worker_processes auto;
pid /run/nginx.pid;
#include /etc/nginx/modules-enabled/*.conf;

events {
        worker_connections 768;
        # multi_accept on;
}

http {

        ##
        # Basic Settings
        ##

        sendfile on;
        tcp_nopush on;
        tcp_nodelay on;
        reset_timedout_connection on;
        client_body_timeout 10;
        send_timeout 2;

        keepalive_timeout 30;
        keepalive_requests 100000;

        client_max_body_size 10G;


        include /etc/nginx/mime.types;
        default_type application/octet-stream;

        ##
        # SSL Settings
        ##

        ssl_protocols TLSv1 TLSv1.1 TLSv1.2; # Dropping SSLv3, ref: POODLE
        ssl_prefer_server_ciphers on;

        ##
        # Logging Settings
        ##

        access_log /var/log/nginx/access.log;
        error_log /var/log/nginx/error.log;

        ##
        # Gzip Settings
        ##

        #gzip on;

        #gzip_comp_level  2;
        #gzip_min_length  1000;
        #gzip_proxied     expired no-cache no-store private auth;
        #gzip_types       text/plain application/x-javascript text/xml text/css application/xml;

        include /etc/nginx/conf.d/*.conf;

}
```

```shell



server
{
        
        listen 80;

        server_name  exhibit-nginx.usagecorpus.com;

        location / {
             add_header Content-Type text/plain;
             return 200 'okay';           

        }


}

```

```shell

upstream  EXNGINX{
        ip_hash;
        #least_conn;

        server  127.0.0.1:8000;
        server  127.0.0.1:8001;

}

server
{
        listen       80;
        server_name      exhibit-nginx.usagecorpus.com;
        return 301 https://$host$request_uri;
}



server
{
        
        server_name  exhibit-nginx.usagecorpus.com;

        location / {
             #add_header Content-Type text/plain;
             #return 200 'okay';           

             proxy_pass http://EXNGINX;
             proxy_set_header X-Real-IP $remote_addr;
             proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
             proxy_set_header X-Forwarded-For $remote_addr;
             proxy_set_header X-Forwarded-Proto $scheme;
             proxy_set_header Host $http_host;

             proxy_read_timeout 300s;
             proxy_connect_timeout 75s;
             
             proxy_buffering off;
             proxy_request_buffering off;

             proxy_http_version 1.1;
             proxy_set_header   Upgrade $http_upgrade;
             proxy_set_header   Connection "upgrade";
        }







    #listen 80; # managed by Certbot

    listen 443 ssl; # managed by Certbot
    ssl_certificate /etc/letsencrypt/live/exhibit-nginx.usagecorpus.com/fullchain.pem; # managed by Certbot
    ssl_certificate_key /etc/letsencrypt/live/exhibit-nginx.usagecorpus.com/privkey.pem; # managed by Certbot
    include /etc/letsencrypt/options-ssl-nginx.conf; # managed by Certbot
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem; # managed by Certbot

}
```


```shell
#!/bin/bash


while true; do nc -l 0.0.0.0 8000 < resp; done


```


# CMAKELISTS TXT CMAKE

```shell

cd $PROJECT_DIR

mkdir -p build

cd build

cmake ..

make

# blah blah after that


```
```shell
# build lib


cmake_minimum_required(VERSION 3.2)
project(dynamiclink_print)
set(CMAKE_BUILD_TYPE Release)

include_directories(include)

set(SOURCES src/crazything.cc)


# target lib for .so
add_library(crazything SHARED ${SOURCES})
# target lib for .a
add_library(something STATIC ${SOURCES})

# for installing lib: sudo make install...
install(TARGETS crazything DESTINATION /usr/lib)


```


```shell
# build target

# min ver
cmake_minimum_required(VERSION 3.2)
# project name
project (main_lib_print)


# include
include_directories(
    ../2_staticlink_print/include
    ../3_dynamiclink_print/include
    )

# set var
set ( PROJECT_LINK_LIBS libsomething.a libcrazything.so)

# static or dynamic link dirs
link_directories( 
    ../2_staticlink_print/build
    ../3_dynamiclink_print/build 
    )


# target bin
add_executable(main.out main.cc)

# linking to target bin
target_link_libraries(main.out ${PROJECT_LINK_LIBS})

```

```shell

# include another one

include(another/cmakelists.txt)



# source all
file(
  GLOB somefile
  "${SRC_DIR}/*.c"
)

# -pthread

Threads::Threads

# export (when included in another CMakeLists.txt)
install(
  TARGETS thethinghere
  EXPORT thethinghere
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)

install(
  EXPORT thethinghere
  DESTINATION lib
  FILE agvcancomm-Config.cmake
)



```


# JENKINSFILE


```shell

pipeline{

    agent any

        environment {

            // Target Path

            TARGET_PATH="/home/target"
            TARGET_PREFIX="target_"

            // Preparation Stage

            GIT_ADDR="https://${GIT_ID}:${GIT_PW}@github.com/seantywork/nothing.git"
            GIT_BRANCH="main"
            
            // Build Stage

            REG_ADDR="docker.io -u ${REG_ID} -p ${REG_PW}"
            IMG_1="${TARGET_PREFIX}nothing"
            TAG_1="docker.io/seantywork/nothing"


            // Deployment Stage

            DEPLOYMENT_NODE="${DEPLOYMENT_NODE}"

            APP_PATH_1="${TARGET_PATH}/k8s/app.yaml"


            UPDATE_1="deployment/nothing"


            
            // ********* Safe Scripts **********

            // stage Preparation_checkSafe
            // --> USAGE: ${SAFE_PREP} ${TARGET_PATH} 
            SAFE_PREP="/home/safe_preparation"

            // stage Termination_checkSafe
            // --> USAGE: ${SAFE_TERM} ${TAG_1} ${TAG_2} ...
            SAFE_TERM="/home/safe_termination"


        }

    stages{



        stage ('Preparation'){

            steps {

                sh ''' #!/bin/bash

                    mkdir -p ${TARGET_PATH}

                    git -C ${TARGET_PATH} init

                    git -C ${TARGET_PATH} pull ${GIT_ADDR} ${GIT_BRANCH}

                '''

            }

        }

        stage ('Preparation-safety-check'){

            steps {

                sh ''' #!/bin/bash

                    ${SAFE_PREP} ${TARGET_PATH} 

                '''

            }

        }

        stage ('Build'){

            steps {

                sh ''' #!/bin/bash

                    docker login ${REG_ADDR}

                    docker-compose -f "${TARGET_PATH}/docker-compose.yaml" up -d --build

                    docker-compose -f "${TARGET_PATH}/docker-compose.yaml" down

                    docker tag ${IMG_1} ${TAG_1}

                    docker push ${TAG_1}

                '''

            }

        }

        stage ('Deployment'){

            steps {

                sh ''' #!/bin/bash

                    scp ${APP_PATH_1} "${DEPLOYMENT_NODE}:/home/src"

                    ssh ${DEPLOYMENT_NODE} "kubectl -n operation apply -f app.yaml"

                    ssh ${DEPLOYMENT_NODE} "kubectl -n operatioin rollout restart ${UPDATE_1}"

                    ssh ${DEPLOYMENT_NODE} "rm ./app.yaml"

                '''
            }

        }

        stage ('Termination'){

            steps {

                sh ''' #!/bin/bash

                    rm -rf ${TARGET_PATH}

                '''


            }

        }

        stage ('Termination-safety-check'){

            steps {

                sh ''' #!/bin/bash

                    ${SAFE_TERM} ${TAG_1} 

                '''

            }


        }


    }
}


```



# GITHUB WORKFLOW

```yaml

name: self hosted runner

on :
  push:
    branches: [ main ]

jobs:

  uc_self_hosted_build:
    runs-on: self-hosted
    steps: 
      - run: mkdir -p /root/uc
      - run: git -C /root/uc init
      - run: git -C /root/uc pull https://github.com/seantywork/uc.git 
      - run: docker-compose -f /root/uc/docker-compose.yaml up -d --build
      - run: docker-compose -f /root/uc/docker-compose.yaml down
      - run: docker tag uc_traffic seantywork/traffic
      - run: docker tag uc_web seantywork/web
      - run: docker tag uc_db seantywork/db
      - run: docker push seantywork/traffic
      - run: docker push seantywork/web
      - run: docker push seantywork/db
      - run: rm -r /root/uc

  uc_self_hosted_deployment:
    needs: uc_self_hosted_build
    runs-on: self-hosted
    steps:
      - run: ssh $SSH_XY "git -C /home/seantywork/uc pull "
      - run: ssh $SSH_XY "kind delete cluster --name kindcluster"
      - run: ssh $SSH_XY "kind create cluster --name kindcluster --config /home/seantywork/uc/k8s/kindcluster.yaml --image=kindest/node:v1.27.2"
      - run: ssh $SSH_XY "kubectl create namespace uc"
      - run: ssh $SSH_XY "kubectl create secret generic docker-secret --from-file=.dockerconfigjson=/root/.docker/config.json --type=kubernetes.io/dockerconfigjson -n uc"
      - run: ssh $SSH_XY "kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml"
      - run: sleep 3
      - run: echo "waiting...1"
      - run: sleep 3
      - run: echo "waiting...2"
      - run: sleep 3
      - run: echo "waiting...3"
      - run: sleep 3
      - run: echo "waiting...4"
      - run: sleep 3
      - run: echo "waiting...5"
      - run: ssh $SSH_XY "kubectl -n uc apply -f /home/seantywork/uc/k8s/app.yaml"
 
  uc_self_hosted_service:
    needs: uc_self_hosted_deployment
    runs-on: self-hosted
    steps:
      - run: sleep 3
      - run: echo "waiting...1"
      - run: sleep 3
      - run: echo "waiting...2"
      - run: sleep 3
      - run: echo "waiting...3"
      - run: sleep 3
      - run: echo "waiting...4"
      - run: sleep 3
      - run: echo "waiting...5"
      - run: ssh $SSH_XY "kubectl -n uc apply -f /home/seantywork/uc/k8s/ingress.yaml"
  

```


# REDIS

```yaml
version: "3"

services:

    redis:
      build: ./redis
      container_name: redis
      volumes:
        - ./redis/data:/var/lib/redis/data
        - ./redis/redis.conf:/usr/local/etc/redis/redis.conf
      ports:
        - "6379:6379"


```


```shell

FROM redis:7.0.8

EXPOSE 6379

CMD ["/bin/sh","-c","redis-server","--requirepass","youdonthavetoknow"]


```


```shell

port              6379
daemonize         yes
save              60 1
bind              0.0.0.0
tcp-keepalive     300
dbfilename        dump.rdb
dir               ./
rdbcompression    yes

```


# REMOTE DESKTOP

```shell

# remote server

sudo apt install xrdp -y

sudo systemctl restart xrdp


# client

sudo apt install remmina remmina-plugin-vnc



```

# GRPC SET UP


```shell
# install

export MY_gRPC_DIR=$HOME/.local

mkdir -p $MY_gRPC_DIR

export PATH="$MY_gRPC_DIR/bin:$PATH"

# cmake version should be 3.8+


sudo apt install -y build-essential autoconf libtool pkg-config

git clone --recurse-submodules -b v1.60.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc


cd grpc
mkdir -p cmake/build
pushd cmake/build
cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_gRPC_DIR \
      ../..
make -j 4
sudo make install
popd


```


```shell

# build

# src CMakeLists.txt and common.cmake update

export MY_gRPC_DIR=$HOME/.local

mkdir -p build

cd build && cmake -DCMAKE_PREFIX_PATH=$MY_gRPC_DIR ..

make -j 4


```


# ROS 2

```shell

# on ubuntu 22.04 ros2 humble hawksbill

# takes up about 2G disk space

locale  # check for UTF-8

sudo apt update && sudo apt install locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

locale  # verify settings



sudo apt install software-properties-common
sudo add-apt-repository universe


sudo apt update && sudo apt install curl -y
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg


echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null




sudo apt update


sudo apt upgrade


sudo apt install ros-humble-desktop


sudo apt install ros-humble-ros-base


sudo apt install ros-dev-tools



source /opt/ros/humble/setup.bash


# check if installation is succeful


# terminal 1

source /opt/ros/humble/setup.bash
ros2 run demo_nodes_cpp talker


# terminal 2

source /opt/ros/humble/setup.bash
ros2 run demo_nodes_py listener


```


```shell

# cartographer

ros2 pkg list |grep cartographer

# if none

sudo apt install ros-$ROS_DISTRO-cartographer -y

# turtlebot3

sudo apt install ros-$ROS_DISTRO-turtlebot3* -y

# simulation

source /opt/ros/humble/setup.bash

export TURTLEBOT3_MODEL=burger

export GAZEBO_MODEL_PATH=`ros2 pkg \
prefix turtlebot3_gazebo`/share/turtlebot3_gazebo/models/


ros2 launch turtlebot3_gazebo turtlebot3_world.launch.py

# troubleshoot
# not sure if effective?
gazebo -s libgazebo_ros_init.so -s libgazebo_ros_factory.so myworld.world


# on terminal 2 remote pc: teleoperation node

source /opt/ros/humble/setup.bash

ros2 run turtlebot3_teleop teleop_keyboard

# on terminal 3 remote pc: cartographer

source /opt/ros/humble/setup.bash

ros2 launch turtlebot3_cartographer \
cartographer.launch.py \
use_sim_time:=True

```


```shell

# real

# on turtlebot3 do below

source .bashrc

cd turtlebot3_ws

#Set up ROS_DOMAIN_ID

export ROS_DOMAIN_ID="Your Number"

# e.g. export ROS_DOMAIN_ID=11

source install/setup.bash

ros2 launch turtlebot3_bringup robot.launch.py

# on terminal2 : teleoperation node


source /opt/ros/humble/setup.bash

export ROS_DOMAIN_ID="Your Number"
# e.g. export ROS_DOMAIN_ID=11

ros2 run turtlebot3_teleop teleop_keyboard


# on terminal3 : cartoprapher



source /opt/ros/humble/setup.bash

export ROS_DOMAIN_ID="Your Number"
# e.g. export ROS_DOMAIN_ID=11

ros2 run cartographer_ros occupancy_grid_node -resolution 0.05\
-publish_period_sec 1.0

ros2 run cartographer_ros cartographer_node\
-configuration_directory]\ 
install/turtlebot3_cartographer/share/turtlebot3_cartographer\
/config -configuration_basename turtlebot3_lds_2d.lua


```

# ROS API

```shell
# term1

mkdir -p ros2_ws/src


source /opt/ros/humble/setup.bash

# git clone https://github.com/ros/ros_tutorials.git -b humble

# term1 dependency at ros2_ws

rosdep install -i --from-path src --rosdistro humble -y

# term1 build at ros2_ws

colcon build --symlink-install

# term2

source /opt/ros/humble/setup.bash

# term2 at ros2_ws

source install/local_setup.bash

```

```shell

# term1

# package dev

source /opt/ros/humble/setup.bash

# package dec at ros2_ws/src

ros2 pkg create --build-type ament_cmake --node-name $NODE_NAME $PKG_NAME

# source goes to ros2_ws/src/$PKG_NAME/src

# package.xml, CMakeLists.txt update

# install dependency at ros2_ws

rosdep install -i --from-path src --rosdistro humble -y

# package build at ros2_ws

colcon build --symlink-install --packages-select $PKG_NAME



# term2

source /opt/ros/humble/setup.bash

# term2 at ros2_ws

source install/local_setup.bash

# run

ros2 run $PKG_NAME $NODE_NAME



```
# ROS COLCON BUILD LAUNCH

```shell
# term 1

source /opt/ros/directory/setup.bash

colcon build --symlink-install

```

```shell
# term 2

source install/setup.bash

ros2 launch $PAKCAGE_NAME $LAUNCHFILE_NAME


```


# ROS DOCKER

```shell

# pyslam v2

nvidiactk

# config.sh work dir 

build.sh <NAME>

xhost +

run.sh <NAME>

# $DISPLAY ?



```


# CAN NETWORK

```shell

# using gpio

sudo apt-get install busybox


# register up for a test

sudo busybox devmem 0x0c303000 32 0x0000C400
sudo busybox devmem 0x0c303008 32 0x0000C458
sudo busybox devmem 0x0c303010 32 0x0000C400
sudo busybox devmem 0x0c303018 32 0x0000C458


sudo modprobe can
sudo modprobe can_raw
sudo modprobe mttcan


# each second line optional?

sudo ip link set can0 type can bitrate 500000 \
    dbitrate 2000000 berr-reporting on fd on
sudo ip link set can1 type can bitrate 500000 \
    dbitrate 2000000 berr-reporting on fd on


sudo ip link set up can0
sudo ip link set up can1


# cron the sh file at reboot

```
```shell

# vcan

sudo modprobe vcan

ip link add dev vcan0 type vcan

ip link set up vcan0

```

```shell
# can utils

sudo apt-get install can-utils

# send to if with id
cansend vcan0 101#41424344

# dump all incoming data

candump -t Absolute vcan0

# send random frame continuously

cangen vcan0

```

# MQTT

```shell
# docker compose

version: "3.7"

services:
  mosquitto:
    image: eclipse-mosquitto
    container_name: mosquitto
    restart: unless-stopped
    ports:
      - "1883:1883"
      - "9001:9001"
    volumes:
      - ./mosquitto:/etc/mosquitto
      - ./mosquitto/mosquitto.conf:/mosquitto/config/mosquitto.conf

```


```shell

# volume config

listener 1883
allow_anonymous true
# password_file /etc/mosquitto/passwd

```

# RASPBERRY PI

```shell
# gpio pins

| #  |           | PIN  |    |    | PIN  |          | #  |
|----|-----------|------|----|----|------|----------|----|
| 1  | 3v3 Power | `1`  | 🟧 | 🔴 | `2`  | 5v Power | 20 |
| 2  | GPIO 2    | `3`  | 🔵 | 🔴 | `4`  | 5v Power | 19 |
| 3  | GPIO 3    | `5`  | 🔵 | ⚫ | `6`  | *Ground* | 18 |
| 4  | GPIO 4    | `7`  | 🟢 | 🟣 | `8`  | GPIO 14  | 17 |
| 5  | *Ground*  | `9`  | ⚫ | 🟣 | `10` | GPIO 15  | 16 |
| 6  | GPIO 17   | `11` | 🟢 | 🟤 | `12` | GPIO 18  | 15 |
| 7  | GPIO 27   | `13` | 🟢 | ⚫ | `14` | *Ground* | 14 |
| 8  | GPIO 22   | `15` | 🟢 | 🟢 | `16` | GPIO 23  | 13 |
| 9  | 3v3 Power | `17` | 🟠 | 🟢 | `18` | GPIO 24  | 12 |
| 10 | GPIO 10   | `19` | 🟡 | ⚫ | `20` | *Ground* | 11 |
| 11 | GPIO 9    | `21` | 🟡 | 🟢 | `22` | GPIO 25  | 10 |
| 12 | GPIO 11   | `23` | 🟡 | 🟡 | `24` | GPIO 8   | 9  |
| 13 | *Ground*  | `25` | ⚫ | 🟡 | `26` | GPIO 7   | 8  |
| 14 | GPIO 0    | `27` | 🔵 | 🔵 | `28` | GPIO 1   | 7  |
| 15 | GPIO 5    | `29` | 🟢 | ⚫ | `30` | *Ground* | 6  |
| 16 | GPIO 6    | `31` | 🟢 | 🟢 | `32` | GPIO 12  | 5  |
| 17 | GPIO 13   | `33` | 🟢 | ⚫ | `34` | *Ground* | 4  |
| 18 | GPIO 19   | `35` | 🟤 | 🟢 | `36` | GPIO 16  | 3  |
| 19 | GPIO 26   | `37` | 🟢 | 🟤 | `38` | GPIO 20  | 2  |
| 20 | *Ground*  | `39` | ⚫ | 🟤 | `40` | GPIO 21  | 1  |

**Legend:**

* 🟢 GPIO (General Purpose IO)
* 🟡 SPI (Serial Peripheral Interface)
* 🔵 I2C (Inter-integrated Circuit)
* 🟣 UART (Universal Asyncronous)
* 🟤 PCM (Pulse Code Modulation)
* ⚫ Ground
* 🔴 5v (Power)
* 🟠 3.3v (Power)

```
```shell

# pgpio

# install pigpio
wget https://github.com/joan2937/pigpio/archive/master.zip
unzip master.zip
cd pigpio-master
make
sudo make install


# ir receiver

# S
# > OUT
# > POWER
# > GND
# -

# OUT to GPIO 4
# POWER to PIN 1
# GND to PIN 9
```
```shell
# lirc

# ir transmitter

# > DAT
# > VCC
# > GND

# DAT to GPIO 23
# VCC to PIN 2
# GND to PIN 20

# install lirc

sudo apt-get update 

sudo apt-get install lirc -y


```shell
# on xU22.04
# /boot/firmware/config.txt
# below last [all]

[all]
...
dtoverlay=gpio-ir,gpio_pin=4
dtoverlay=gpio-ir-tx,gpio_pin=23
...

# 4 receive < in

# 23 transmit > out
```

```shell
# /etc/lirc/hardware.conf

LIRCD_ARGS="--uinput --listen"
LOAD_MODULES=true
DRIVER="default"
DEVICE="/dev/lirc0"
MODULES="lirc_rpi"

# /etc/lirc/lirc_options.conf

#driver         = devinput
#device         = auto
driver          = default
device          = /dev/lirc0
```
```shell

# check incoming infrared signal

sudo systemctl stop lircd

sudo mode2 -m -d /dev/lirc1 > *.lirc.conf

# *.lirc.conf format

begin remote

  name LG_AC
  flags RAW_CODES|CONST_LENGTH
  eps 30
  aeps 100

  gap 513079

      begin raw_codes

  name AC_ON

     3044     9814      495     1569      465      548
      469      548      469      550      467     1592
      441      549      525      496      494      520
      466      551      469      548      469      550
      467      548      465      577      468      549
      466      552      465      550      466      550
      450     1612      468     1566      443     1620
      418      571      467      550      520      497
      498      519      494      523      467     1620
      420     1614      442     1619      464   

  name AC_OFF

     3075     9815      493     1593      442      548
      470      549      470      546      468     1591
      441      575      470      547      467      550
      467     1595      439     1594      439      553
      468      550      465      573      466      551
      444      573      443      574      446      570
      444      572      444      573      443      574
      444      572      444     1617      440      556
      462     1593      445      572      445      575
      468      572      444     1617      418   

      end raw_codes

end remote


# cp

sudo cp lgac.lirc.conf /etc/lirc/lircd.conf.d/


# mv 

sudo mv /etc/lirc/lircd.conf.d/devinput.lircd.conf /etc/lirc/lircd.conf.d/devinput.lircd.conf.bak

# reboot

# list

sudo irsend LIST LG_AC ""

# send

sudo irsend SEND_ONCE LG_AC AC_ON

```



# JETSON XAVIER NX

```shell

# first flash

# recovery mode

# check if recovery mode

lsusb 

# get jetson linux bsp and rootfs

# 35.4.1 (ubuntu20)

# https://developer.nvidia.com/embedded/jetson-linux-r3541

# un bzip2 un tar both and get the rootfs using sudo tar into BSP/rootfs

sudo ./apply_binaries.sh

sudo ./nvautoflash.sh

# now on jetson

fdisk /dev/nvme0n

mkfs.ext4 /dev/nvme0np1


# copy to volume and then switch rootfs when booting

sudo mount /dev/nvme0n1p1 /mnt

sudo rsync -axHAWX --numeric-ids --info=progress2 --exclude={"/dev/","/proc/","/sys/","/tmp/","/run/","/mnt/","/media/*","/lost+found"} / /mnt


sudo cp setssdroot.service /etc/systemd/system
sudo cp setssdroot.sh /sbin
sudo chmod 777 /sbin/setssdroot.sh
systemctl daemon-reload
sudo systemctl enable setssdroot.service


sudo cp /etc/systemd/system/setssdroot.service /mnt/etc/systemd/system/setssdroot.service
sudo cp /sbin/setssdroot.sh /mnt/sbin/setssdroot.sh


sudo touch /etc/setssdroot.conf

```

```shell
# setssdroot.service

[Unit]
Description=Change rootfs to SSD in M.2 key M slot (nvme0n1p1)
DefaultDependencies=no
Conflicts=shutdown.target
After=systemd-remount-fs.service
Before=local-fs-pre.target local-fs.target shutdown.target
Wants=local-fs-pre.target
ConditionPathExists=/dev/nvme0n1p1
ConditionPathExists=/etc/setssdroot.conf
ConditionVirtualization=!container
[Service]
Type=oneshot
RemainAfterExit=yes
ExecStart=/sbin/setssdroot.sh
[Install]
WantedBy=default.target

```

```shell
# setssdroot.sh
#!/bin/sh
NVME_DRIVE="/dev/nvme0n1p1"
CHROOT_PATH="/nvmeroot"

INITBIN=/lib/systemd/systemd
EXT4_OPT="-o defaults -o errors=remount-ro -o discard"

modprobe ext4

mkdir -p ${CHROOT_PATH}
mount -t ext4 ${EXT4_OPT} ${NVME_DRIVE} ${CHROOT_PATH}

cd ${CHROOT_PATH}
/bin/systemctl --no-block switch-root ${CHROOT_PATH}
```

```shell

# install jetpack

sudo apt update
sudo apt install nvidia-jetpack


```


# VIDEO LIVE STREAM FFMPEG RTMP NGINX

```shell

sudo apt install -y ffmpeg v4l-utils

sudo apt install libnginx-mod-rtmp


```
```shell
# /etc/nginx/nginx.conf

# for ssl
stream{

        upstream RTMP_SERVER {
                server 192.168.0.32:1936;
        }


        server {
                listen 1935 ssl;
                proxy_pass RTMP_SERVER;
                ssl_certificate /etc/letsencrypt/live/feebdaed.xyz/fullchain.pem;
                ssl_certificate_key /etc/letsencrypt/live/feebdaed.xyz/privkey.pem;
        }
}

rtmp {
        server {
                listen 1936;
                allow publish 192.168.0.32;

                # allow publish feebdaed.xyz

                application live {
                        live on;
                        interleave on;
                        record off;
                        hls on;
                        hls_path /tmp/hls;
                        hls_fragment 15s;

                        # external auth server
                        on_publish http://localhost:5000/auth;
                        notify_method get;

                }
        }
}

# /etc/nginx/conf.d/hls.conf

server
{

        listen      8080 ssl;
        ssl_certificate /etc/letsencrypt/live/feebdaed.xyz/fullchain.pem; # managed by Certbot
        ssl_certificate_key /etc/letsencrypt/live/feebdaed.xyz/privkey.pem; # managed by Certbot
        include /etc/letsencrypt/options-ssl-nginx.conf; # managed by Certbot
        ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem; # managed by Certbot

        location /hls {
            types {
                application/vnd.apple.mpegurl m3u8;
                video/mp2t ts;
            }
            root /tmp;
            add_header Cache-Control no-cache;

            # for file:// testing only
            add_header Access-Control-Allow-Origin *;

        }
}


```


```shell

# stream to server


ffmpeg -f v4l2 -i /dev/video0 \
  -f alsa -i hw:0,0 \
  -c:v libx264 -pix_fmt yuv420p -framerate 15 -g 30 -b:v 500k \
  -c:a aac -b:a 128k -ar 44100 -ac 2 \
  -f flv -flvflags no_duration_filesize \
  "rtmps://$HOST/live/MyWebCam?puser=$PUSER"

#  -preset ultrafast -tune zerolatency                             \

```

