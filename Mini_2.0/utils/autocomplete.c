#include "../minishell.h"

char		**command_names = NULL;

// Función auxiliar para liberar la memoria de command_names
void	free_command_names(void)
{
	int	i;

	if (command_names)
	{
		i = 0;
		while (command_names[i])
		{
			free(command_names[i]);
			i++;
		}
		free(command_names);
		command_names = NULL;
	}
}
static int	init_builtin_commands(void)
{
	int	i;

	i = 0;
	command_names[i++] = ft_strdup("cd");
	command_names[i++] = ft_strdup("echo");
	command_names[i++] = ft_strdup("pwd");
	command_names[i++] = ft_strdup("export");
	command_names[i++] = ft_strdup("unset");
	command_names[i++] = ft_strdup("env");
	command_names[i++] = ft_strdup("exit");
	return (i);
}

static int	command_exists(char *name, int count)
{
	int	k;

	k = 0;
	while (k < count)
	{
		if (strcmp(command_names[k], name) == 0)
			return (1);
		k++;
	}
	return (0);
}

static void	scan_directory(char *path, int *i)
{
	DIR				*dir;
	struct dirent	*entry;
	char			full_path[1024];

	dir = opendir(path);
	if (!dir)
		return ;
	while ((entry = readdir(dir)) && *i < 99)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
		if (access(full_path, X_OK) == 0 && !command_exists(entry->d_name, *i))
			command_names[(*i)++] = ft_strdup(entry->d_name);
	}
	closedir(dir);
}
static void	load_path_commands(char **env, int *i)
{
	char	*path_env;
	char	**paths;
	int		j;

	path_env = get_env_var("PATH", env);
	j = 0;
	if (!path_env)
		return ;
	paths = ft_split(path_env, ':');
	if (!paths)
		return ;
	while (paths[j])
		scan_directory(paths[j++], i);
	ft_free_split(paths);
}

// Inicializa la lista de comandos para autocompletar
void	initialize_command_names(char **env)
{
	int	i;

	i = 0;
	free_command_names();
	command_names = malloc(sizeof(char *) * 100);
	if (!command_names)
		return ;
	i = init_builtin_commands();
	load_path_commands(env, &i);
	command_names[i] = NULL;
}

// Función de generador para completar comandos
char	*command_generator(const char *text, int state)
{
	char	*name;

	static int list_index, len;
	if (!state)
	{
		list_index = 0;
		len = strlen(text);
	}
	while (command_names && command_names[list_index])
	{
		name = command_names[list_index];
		list_index++;
		if (strncmp(name, text, len) == 0)
			return (ft_strdup(name));
	}
	return (NULL);
}

// Función para concatenar path de forma segura
char	*safe_path_join(const char *dir, const char *file)
{
	size_t	dir_len;
	size_t	file_len;
	char	*result;
	int		need_separator;
	size_t	total_len;

	dir_len = strlen(dir);
	file_len = strlen(file);
	need_separator = 0;
	if (dir_len > 0 && dir[dir_len - 1] != '/' && file_len > 0
		&& file[0] != '/')
		need_separator = 1;
	total_len = dir_len + file_len + need_separator + 1;
	result = malloc(total_len);
	if (!result)
		return (NULL);
	strcpy(result, dir);
	if (need_separator)
		strcat(result, "/");
	strcat(result, file);
	return (result);
}

// Función generadora para archivos y directorios
char	*file_generator(const char *text, int state)
{
	static DIR		*dir;
	static char		*directory;
	static char		*filename;
	static int		len;
	struct dirent	*entry;
	char			*full_path;
	char			*result;
	char			*last_slash;
	int				dir_len;
			struct stat st;
	int				is_dir;
				char cwd[PATH_MAX];
				char *check_path;
	size_t			path_len;

	if (!state)
	{
		if (dir)
		{
			closedir(dir);
			dir = NULL;
		}
		if (directory)
		{
			free(directory);
			directory = NULL;
		}
		last_slash = strrchr(text, '/');
		if (last_slash)
		{
			dir_len = last_slash - text + 1;
			directory = malloc(dir_len + 1);
			if (!directory)
				return (NULL);
			strncpy(directory, text, dir_len);
			directory[dir_len] = '\0';
			filename = last_slash + 1;
			if (directory[0] == '\0')
			{
				free(directory);
				directory = ft_strdup("./");
			}
		}
		else
		{
			directory = ft_strdup("./");
			filename = (char *)text;
		}
		dir = opendir(directory);
		if (!dir)
		{
			free(directory);
			directory = NULL;
			return (NULL);
		}
		len = strlen(filename);
	}
	if (!dir)
		return (NULL);
	while ((entry = readdir(dir)) != NULL)
	{
		if ((strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name,
					"..") == 0) && (len == 0 || (filename[0] != '.' && len == 1)
				|| (filename[0] == '.' && filename[1] != '.' && len == 1)))
			continue ;
		if (strncmp(entry->d_name, filename, len) == 0)
		{
			if (strcmp(directory, "./") == 0 && strcmp(text, "./") != 0
				&& strncmp(text, "./", 2) != 0)
			{
				full_path = ft_strdup(entry->d_name);
			}
			else
			{
				full_path = safe_path_join(directory, entry->d_name);
			}
			if (!full_path)
				continue ;
			is_dir = 0;
			if (full_path[0] == '/')
			{
				if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode))
					is_dir = 1;
			}
			else
			{
				if (getcwd(cwd, sizeof(cwd)) == NULL)
				{
					free(full_path);
					continue ;
				}
				if (strncmp(full_path, "./", 2) == 0)
					check_path = safe_path_join(cwd, full_path + 2);
				else
					check_path = safe_path_join(cwd, full_path);
				if (check_path)
				{
					if (stat(check_path, &st) == 0 && S_ISDIR(st.st_mode))
						is_dir = 1;
					free(check_path);
				}
			}
			if (is_dir)
			{
				path_len = strlen(full_path);
				result = malloc(path_len + 2);
				if (!result)
				{
					free(full_path);
					continue ;
				}
				strcpy(result, full_path);
				if (result[path_len - 1] != '/')
					strcat(result, "/");
				free(full_path);
				return (result);
			}
			return (full_path);
		}
	}
	closedir(dir);
	dir = NULL;
	free(directory);
	directory = NULL;
	return (NULL);
}

// Función de completado para readline
char	**command_completion(const char *text, int start, int end)
{
	(void)end;
	rl_attempted_completion_over = 1;
	if (start == 0)
		return (rl_completion_matches(text, command_generator));
	else
		return (rl_completion_matches(text, file_generator));
}

// Función para configurar el autocompletado
void	setup_autocomplete(char **env)
{
	initialize_command_names(env);
	rl_attempted_completion_function = command_completion;
}