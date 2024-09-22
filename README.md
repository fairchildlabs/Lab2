# Lab2

## 2.1 Create Local (LABPC) development environment

Create a directory "labshare" on the LABPC, then right-click on it and choose "properties" and then "Sharing" and click "Share" button, pick "everyone" and "add" then "Share" button.

```
$ sudo nano /etc/fstab


** change the line like this: //OTIS-FAIRCHILD-/labshare /var/www/html/labshare  cifs  credentials=/var/www/.labsharecred,file_mode=0777,dir_mode=0777,noperm,iocharset=utf8 0
** make it match the name of your LABPC
** <CTRL><X> then pick "Y" to save file

$ sudo reboot now

** Wait for reboot 

$ 

```

## 2.2 Modifications to scootd C Program

### Create Threads
https://copilot.microsoft.com/sl/kowr2FsQVfE


