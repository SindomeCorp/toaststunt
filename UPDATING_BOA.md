Check with any coders before redoing the installation to prevent them from losing any work.  They should move their code to prod or back it up.

## Moving DB

Use the bash script on prod to copy the DB over

## GZIP the files on prod

tar --exclude='./files/eventlog-04-01-17.log' -zcvf ~/files_MM_DD_YY.tar.gz files
scp -i slither.pem files_MM_DD_YY.tar.gz ubuntu@ec2-54-173-190-176.compute-1.amazonaws.com:/home/ubuntu

Unzip:  tar xvzf file.tar.gz

## Rsync the files from prod to boa (UNTESTED)
The boa .pem file should be present as slither.pem on prod, but if not:

scp slither.pem slither@sindome.org:/users/slither

To rsync the files (UNTESTED!!):

rsync -ravz -e "ssh -i slither.pem" /Users/sindome/Servers/sindome-moo/files ubuntu@ec2-54-173-190-176.compute-1.amazonaws.com:/home/ubuntu

## Restart MOO
./restart.sh sindome 5555

## Set Welcome Message

;#10.welcome_message = #10.welcome_message_boa

## Promote Coders to $creator / @programmer

// kronos

## Reset Player Passwords
;for p in ($ou:descendents(#131)) if (!$wiz_utils:is_admin(p)) clear_property(p, "password"); endif endfor

## stop slack trying to send
@program $browser:_post and make it return at top

## unschedule some stuff
;$scheduler:remove_scheduled(#18056, "pull_unread_gridmail")
;$scheduler:remove_scheduled(#24, "get_blocked_proxy_ips")
;$scheduler:remove_scheduled(#61180, "attention_nanny)

# Updating Webclient
cp ~/favicon-boa.ico /dome-client.js/public
