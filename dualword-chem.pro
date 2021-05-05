lessThan(QT_VERSION, 4.5) {
	error('Qt 4.5+ required...')
}

TEMPLATE = subdirs
SUBDIRS = src