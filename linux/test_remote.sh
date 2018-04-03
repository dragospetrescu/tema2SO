#!/usr/bin/env bash

scp linux/cmd.c student@172.16.245.130:/home/student/Tema2
ssh student@172.16.245.130 /home/student/Tema2/run_remote.sh
