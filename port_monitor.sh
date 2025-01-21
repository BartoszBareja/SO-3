#!/bin/bash

LOG_FILE="/var/log/open_ports.log"
CHECK_INTERVAL=60

log_message() {
    local message="$1"
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] $message" >> "$LOG_FILE"
}

get_open_ports() {
    ss -tuln 2>/dev/null | awk 'NR>1 {split($5, a, ":"); print a[length(a)]}' | sort -u
}

main() {
    log_message "Starting port monitoring daemon."
    declare -A previous_ports

    while true; do
        current_ports=($(get_open_ports))
        declare -A current_ports_map

        for port in "${current_ports[@]}"; do
            current_ports_map["$port"]=1
            if [[ -z "${previous_ports["$port"]}" ]]; then
                log_message "Port opened: $port"
            fi
        done

        for port in "${!previous_ports[@]}"; do
            if [[ -z "${current_ports_map["$port"]}" ]]; then
                log_message "Port closed: $port"
            fi
        done

        previous_ports=("${current_ports_map[@]}")
        sleep "$CHECK_INTERVAL"
    done
}

# Daemonize process
if [[ "$1" != "--foreground" ]]; then
    nohup "$0" --foreground > /dev/null 2>&1 &
    exit 0
fi

main
