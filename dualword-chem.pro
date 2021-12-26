lessThan(QT_VERSION, 5.0) {
	error('Qt 5.0+ required...')
}

TEMPLATE = subdirs
SUBDIRS = src