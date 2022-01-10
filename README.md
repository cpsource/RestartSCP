Ever have trouble moving large files? As in, StarLink drop-outs, or you live
in the country and suffer with DSL.

(Rant: Egad! The clowns in Washington, D.C. think DSL satisifies the need for one of America's
critical modern infrastructures, the internet. Well, DSL doesn't.)

Then, this package is for you. Bash shell scripts will break up files into chunks,
transfer those chunks via scp, and then reconstruct the origional file at the target
machine. Restart capability is provided to get you past internet errors.

Here's how to use RestartSCP:

1. Get a copy of this repository

   git clone https://github.com/cpsource/RestartSCP
   
2. Create a target directory on your target machine. This is the machine that will
   be receiving the file. For this example, we'll use tmp.

   ssh <target-username>:<target-system-name>:
   mkdir tmp
   exit

3. Create a source directory to split up the file you wish to send.

   mkdir tmp
   cd tmp
   cp ../RestartSCP/* .

4. Populate your source directory

   ./setup.sh .. my-big-file.iso <target-username>:<target-system-name>:~/tmp/.

   Where
	The first argument is the directory that contains my-big-file.iso
	The second argument is the file to copy in that directory.
	The third argument is the target destination of the copy
	
Be sure to add your ~/.ssh/id_rsa.pub to to <target-system-name>:~/.ssh/authorized_keys
beforehand. Doing this is beyond the scope of this document.

5. Start your copy operations

   ./cp-restart.sh

To quit for any reason, say, you wanted to use the internet at all for something else, just type
^c a few times to exit the script.

6. When done (in my case, 36 hours later), reconstruct the file at the target

   ssh <target-username>:<target-system-name>
   cd tmp
   ./rebuild.sh

The file cleanup is left to you. Make sure the file is reconstructed properly
before you remove the xa* files.

On the target, you can also run

   ./tst.sh

which checks all the segments with sha256sum.

If you have to restart for any reason, do the following

   ./restart
   ./cp-restart.sh

and you are on your way.

Requirements
------------

Your source machine will need these packages installed:

     gcc
     sha256sum
     bash
     git
     
You can pickup the latest RestartSCP by

    cd RestartSCP
    git pull

