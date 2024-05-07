#/bin/bash

ssh operator1 "python3 /home/phnxrc/samfred/db/view_cosmics.py $@"
root -l plot_cosmics.C
