9th Edition VM no longer available, this is the 10th edition VM.

http://cs.westminstercollege.edu/~greg/osc10e/vm/index.html

command-line based Ubuntu Server running version 4.4 of the Linux kernel. 
software, including:

- gcc
- make
- java 8
- python
- ssh

Home dir. has course source code + README.

username+password=osc+osc

* Moving Files Between  the Host and the Virtual Machine
  * wget
  * to move files in/out setup port forward.
  * name = ssh
   - protocol = tcp 
   - Host IP = 127.0.0.1 
   - Host port = 2222 
   - Guest IP = 10.0.2.15 
   - Guest port = 22
   * to access VM - sftp -P 2222 osc@127.0.0.1
  
* Building EDK2 EmulatorPkg.

* Tried http://linuxseekernel.blogspot.com/2015/07/uefiedk2-emulatorpkg-in-ubuntu.html
  * Got build error "gcc: error: brotli/c/tool/brotli.o"
  * spent 4 hrs trying to fix this error.
  * checking out edk2 about a year ago - BaseTools now builds.
  * cf2d8d4978e87a80d23ecd40ef7b49f27199a13c
  * https://github.com/tianocore/edk2/commit/cf2d8d4978e87a80d23ecd40ef7b49f27199a13c
  
* Enabling 
  * VM drag/drop
  * shared folders
  * the clipboard.
  * command line scrolling

* To ssh into the linux VM use : `ssh -p 2222 osc@127.0.0.1`
* 

* to copy paste in ubuntu - control+shift+c/v

