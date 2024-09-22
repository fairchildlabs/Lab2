# Lab2

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
** alias lab="cd /var/www/html/labshare/"

$ sudo reboot now

** Wait for reboot

$ lab

** Should takeyou to /var/www/html/labshare

$ pwd

** Should see astros@mario:/var/www/html/labshare 

$ git clone https://github.com/fairchildlabs/Lab1.git
$ git clone https://github.com/fairchildlabs/Lab2.git
$ cd Lab2/code
$ make
$ make scootdtest



```



## 2.2 Modifications to scootd C Program

### Create Threads
https://copilot.microsoft.com/sl/kowr2FsQVfE


