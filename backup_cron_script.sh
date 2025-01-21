#!/bin/bash
# Ustawienia zmiennych
SOURCE_DIRS="/home/labyklient/CPU_module /home/labyklient/Desktop"
BACKUP_DIR="/backup"
LOG_FILE="/home/labyklient/backup_les/backup.log"
# Uzyskanie bieżącej daty w formacie YYYY-MM-DD
DATE=$(date +"%Y-%m-%d")
# Nazwa pliku kopii zapasowej z datą
BACKUP_FILE="${BACKUP_DIR}/backup_${DATE}.tar.gz"
# Funkcja logująca operacje
log_message() {
  echo "$(date +"%Y-%m-%d %H:%M:%S") - $1" >> $LOG_FILE
}
# Rozpoczęcie tworzenia kopii zapasowej
log_message "Rozpoczęcie tworzenia kopii zapasowej."
# Tworzenie kopii zapasowej
tar -czf $BACKUP_FILE $SOURCE_DIRS 2>> $LOG_FILE
if [ $? -eq 0 ]; then
  log_message "Kopia zapasowa została pomyślnie utworzona: $BACKUP_FILE"
else
  log_message "Błąd podczas tworzenia kopii zapasowej!"
fi