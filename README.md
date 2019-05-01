# ld.so.preload

This project represents the creation of a Linux library that prevents race condition, specifically TOCTTOU, attacks. 

Please read this document in order to execute the Proof Of Concept (POC).

Index:
* Context
* POC

## Context

LD_PRELOAD allows dynamically linked function hooking.

You might consider LD_PRELOAD a security hole but it is actually secured since it won't work if ruid != euid. 

Shared libraries specified in **/etc/
ld.so.preload** are loaded **before** those from LD_PRELOAD.

Statistically linked libraries are immune to LD_PRELOAD as well as functions defined within the very source code.

### Here you can find very useful LD_PRELOAD information:

* http://www.goldsborough.me/c/low-level/kernel/2016/08/29/16-48-53-the_-ld_preload-_trick/
* https://rafalcieslak.wordpress.com/2013/04/02/dynamic-linker-tricks-using-ld_preload-to-cheat-inject-features-and-investigate-programs/
* https://blog.fpmurphy.com/2012/09/all-about-ld_preload.html
* https://blog.jessfraz.com/post/ld_preload/
* https://stackoverflow.com/questions/426230/what-is-the-ld-preload-trick

## POC

In order to execute the TOCTTOU POC simply execute the following commands.

There are two POC files. 

* [vulnerableAccessFopen.c](/vulnerableAccessFopen.c) - Vulnerable <access, fopen> sequence.
* [vulnerableLstatOpen.c](/vulnerableLstatOpen.c) - Vulnerable <lstat, open> sequence.

Both files represent TOCTTOU-vulnerable code. The way of exploiting TOCTTOU consists of an attacker with lesser privileges than the actual program creating a file that he/she has access to in first place and then, at the exact moment (called the vulnerability window), replacing that file with a symbolic link towards some critic file he/she has no access to. Since the program will be running with escalated privileges, generally steuid to root, the user will be able to actually read/write in the protected file. 

In order to test TOCTTOU exploitation simply follow these steps: (**Assuming you are working from the same directory where you cloned this repo and from where this README is**)

1. We need some protected file, so let's create one with root user.

``
echo "A root file" | sudo tee r00t.file
``

2. Now let's compile the vulnerable code you want to test. We'll call the binary *vulnerable*. So you execute either of these commands:

``
sudo gcc vulnerableAccessFopen.c -o vulnerable
sudo gcc vulnerableLstatOpen.c -o vulnerable
`` 

3. Now we need to activate setuid bit.

``
sudo chmod u+s vulnerable
``

4. You should now have the following files (amongst others):

``
-rwxr-xr-x 1 user user ... ... .. ..:.. exploit.sh
-rw-r--r-- 1 root root ... ... .. ..:.. r00t.file 
-rwsr-xr-x 1 root root ... ... .. ..:.. vulnerable
``

5. Now, in order to exploit the vulnerable code, simply execute the script:

``
./exploit.sh r00t.file 2> /dev/null
``
*If you want to see every output, edit the script and dont redirect its output. Then simply execute ./exploit.sh r00t.file*

6. Now simply wait for the script to finish. Sooner or later r00t.file will be edited via a *user* created file that's been replaced by a symbolic link at the right moment. Since TOCTTOU vulnerability is a non-deterministic one, we cannot know beforehand how long it will take to actually exploit the vulnerability. There are many factors to take into account, such as CPU performance, OS, enviroment variables, OS preempting, context switches, etc...