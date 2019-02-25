Problem 0 is all about working with disk images and filesystems on Linux.
All these commands will work fine from inside the "nsl" virtual machine.

0.A: Set up screen recording with:
  script -af hw3.0.txt

Everything you type from that terminal should get recorded into
the file hw3.0.txt.  Type ls from that terminal.
Open a second terminal to check that hw3.0.txt shows your ls
(with some formatting control characters around it).

0.B: Use "sudo losetup" to set up /dev/loop0 so you can access 
the file tinydisk.hdd via /dev/loop0.  
(Hint: "man losetup" to figure out how to call it.)

0.C: Use "sudo partprobe" to scan your newly created /dev/loop0 for partitions.

0.D: Use "lsblk -f" to show the partitions.
There should be at least two partitions.

0.D: Use "sudo mount" to mount the second partition (/dev/loop0p2) 
so you can see its files.   It should have directories including 
"long_name" and "permission_denied", and a README.txt file.

0.E: Verify that a normal non-root user can't access permission_denied/topsecret.

0.F: Verify that a root user can access that folder and read the files inside.

0.G: Use "sudo chmod" to make "permission_denied/topsecret" world readable, 
and verify that a non-root user can now read those files.

0.H: Verify that even root can't run "permission_denied/run.sh".

0.I: Use "sudo chmod" to make "permission_denied/run.sh" executable,
and make sure a normal user can run it.

0.J: There's a README.txt file somewhere inside long_name/.  

0.K: There's another README.txt file somewhere inside scavenger/.
Use "ls -i" to examine the inodes for each directory.  Most of them
are linked copies of a decoy directory.  One directory is a real directory,
which you can tell because it has a different inode than the decoys.
After finding your way though two levels of decoys you should find the README.txt.


Upload your hw3.0.txt log file to Blackboard.






