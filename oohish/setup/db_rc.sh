#!/bin/sh
cp pg_hba.conf /etc/postgresql/9.1/main/
/etc/init.d/postgresql reload

su postgres -c "dropdb oohish_db"
su postgres -c "dropuser oohish_admin"
su postgres -c "createdb oohish_db"
su postgres -c "createuser -s -w oohish_admin"
python create_tables.py