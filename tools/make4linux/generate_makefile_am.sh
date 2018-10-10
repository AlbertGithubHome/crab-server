#!/bin/bash
libname=$1
filename="`pwd`/Makefile.am"
echo $filename
echo "llib_LIBRARIES = ${libname}.a" > ${filename}
add=0
function findFile()
{
	for file in `ls`;do
		if [ -d $file ];then
			cd $file
			findFile "$1$file/"
			cd ..
		elif [ -f $file ];then
			if [ ${file##*.} == "cpp" ] || [ ${file##*.} == "c" ];then
				if [ ${add} -eq 0 ];then
					echo ${libname}_a_SOURCES = $1${file} >> ${filename}
				else
					echo ${libname}_a_SOURCES += $1${file} >> ${filename}
				fi
				add=1
			fi
		fi
	done
}
findFile

echo "
INCLUDES =  -I@top_srcdir@/share/
INCLUDES += -I@top_srcdir@/share/detection/
llibdir=@top_srcdir@/share/library/debug
" >> ${filename}
