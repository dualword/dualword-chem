lessThan(QT_VERSION, 6.0) {
        error('Qt 6.0+ required...')
}

TEMPLATE = subdirs
SUBDIRS = src
