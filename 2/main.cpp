#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

//-----------------------------------------------------------------------------
/** Проверяет является ли файл стандартной ссылкой, куоторая автоматически создается при создании директории. Этими стандартными ссылками являются файлы с именем "." и "..".
	@param file - указатель на структуру типа dirent для данного файла.
	@return значение логического выражения о том является ли файл стандартной ссылкой.
 */
int check_is_standard_link(struct dirent* file) {
	return file->d_type == DT_DIR && (
		(strcmp(file->d_name, ".") == 0) || 
		(strcmp(file->d_name, "..") == 0)
	);
}

//-----------------------------------------------------------------------------
/** Проверяет, содержит ли директория subdir другие директории.
	@param subdir - директория для проверки
	@return значенение логического выражения, содержит ли директория под-директории
 */
int check_is_directory_has_directories(DIR* subdir) {
	// Цикл по всем файлам в директории subdir
	while (struct dirent* subdir_file = readdir(subdir)) {
		if (subdir_file->d_type == DT_DIR  && !check_is_standard_link(subdir_file)) {
			return true;
			break;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
int main() {
	DIR* current = opendir(".");

	// Цикл по всем файлам в текущей директории
	while (struct dirent* file = readdir(current)) {
		// Если это директория и не стандартная ссылка, то проверяем её на содержание внутри директорий
		if (file->d_type == DT_DIR && !check_is_standard_link(file)) {
			DIR* subdir = opendir(file->d_name);
			if (check_is_directory_has_directories(subdir))
				printf("%s\n", file->d_name);
			closedir(subdir);
		}
	}

	closedir(current);
}