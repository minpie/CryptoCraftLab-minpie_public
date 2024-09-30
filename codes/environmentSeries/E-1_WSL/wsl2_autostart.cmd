:: This keeps a process running under init
wsl -d WSL2Ubuntu bash -c "nohup bash -c 'while true; do sleep 1h; done &' &>/dev/null "
wsl -d WSL2Ubuntu bash -c "sudo service ssh start"
timeout /t 3