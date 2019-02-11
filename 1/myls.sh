#!/bin/bash

# Данная программа выводит все папки и файлы указанной директорией с информацией о имени, размере, дате создания и числе ссылок для файла.

if [ "$#" -gt "1" ]; then
	# Проверяем количество параметров
	echo "Incorrect number of parameters."
else
	if [ "$1" = "--help" ]; then
		# Выводим справку
		echo "Use \`myls <dir>\` to write directories and files of <dir>."
		echo "Use \`myls --help\` to write this message."
	else
		# Основная работа программы

		# Если число параметров равно нулю, то надо выводить информацию о текущем каталоге
		dir=$1
		if [ $# -eq 0 ]; then
			dir=`pwd`
		fi

		# Проверяем данную директорию на существование
		if [ -d "$dir" ]; then
			# Запоминаем текущий каталог, чтобы в него вернуться
			lastDir=`pwd`
			cd $dir

			dirs=`ls -1 -a -F | grep "/"`

			if [ -n "$dirs" ]; then
				# Выводим список директорий
				echo "-----------Directories:"
				for i in $dirs
				do
					stat --printf="%n\n" $i
				done
			fi

			files=`ls -1 -a -F | grep -v "/"`

			if [ -n "$files" ]; then
				# Выводим список файлов с необходимой информацией
				echo "-----------Files:"
				echo "Name | Size | Date of birth | Number of links"
				for a in $files
				do
					if [ -f "$a" ]; then 
						stat --printf="%n\t%s\t%w\t%h\n" $a
					fi
				done
			fi

			# Возвращаемся в изначальную директорию
			cd $lastDir
		else
			# Сообщение об ошибке
			echo "Directory $c isn't exists."
		fi
	fi
fi
