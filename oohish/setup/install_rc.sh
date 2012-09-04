#!/bin/sh
add-apt-repository -y ppa:pitti/postgresql
add-apt-repository ppa:git-core/ppa
apt-get update

apt-get install --force-yes --yes python
apt-get install --force-yes --yes python-setuptools
apt-get install --force-yes --yes python-software-properties
apt-get install --force-yes --yes python-dev
apt-get install --force-yes --yes postgresql-9.1
apt-get install --force-yes --yes libpq-dev

easy_install psycopg2
