# Lab2
```
(Demonstration)
$      - copy and paste into SSH or PI terminal (only the charecters after the $) 
PI>    - means PI terminal ONLY (Touchscreen or VNC)
//This is a comment in code, for information only, don't type
LABPC>  0This means type this into the cmd window on your labpc
$$$ This means the lines that follow can be copied and pasted all at once (multiple lines)
2$  Means 2nd SSH Terminal 
```

## 2.1 Create Local (LABPC) development environment

Create a directory "labshare" on the LABPC, then right-click on it and choose "properties" and then "Sharing" and click "Share" button, pick "everyone" and "add" then "Share" button.

```
$ sudo nano /etc/fstab

** DEMO
** change the line like this: //OTIS-FAIRCHILD-/labshare /var/www/html/labshare  cifs  credentials=/var/www/.labsharecred,file_mode=0777,dir_mode=0777,noperm,iocharset=utf8 0
** make it match the name of your LABPC
** Alternative, is replace //OTIS-FAIRCHILD with IP Address (from command prompt, ipconfig)
** <CTRL><X> then pick "Y" to save file




$ nano ~/.bashrc

** Add this line to the end of the file, then <CTRL><X> and 'Y' to save
alias lab="cd /var/www/html/labshare/"
<CTRL><X> then pick "Y" to save file


$ sudo reboot now

** Wait for reboot

$ lab

** Should takeyou to /var/www/html/labshare

$ pwd

** Should see astros@mario:/var/www/html/labshare 

$ git clone https://github.com/fairchildlabs/Lab1.git
$ git clone https://github.com/fairchildlabs/Lab2.git
$ cd Lab2/code
$ sudo mkdir /var/www/html/video_13
$ make
$ make scootdtest
$ ./scootd

** We are going to open a second terminal to same pi, Denoted as 2$
2$ lab
2$ cd Lab2/code
2$ ./scootdtest 1

** Observe How Video Captures starts in the other directory

** Open terminal in VNC or directly to PI
PI> lab
PI> cd Lab2/code
PI> python menu.py

** Push stop and start buttons, then see what happens on the scootd program running in the terminal
```



## 2.3 Modifications to scootd C Program

**DEMO -- configure source editor on the LABPC

**DEMO -- use Meld to look at the difference between Lab1 and Lab2 code


