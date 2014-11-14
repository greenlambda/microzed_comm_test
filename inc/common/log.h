
typedef enum log_level{
	LOG_ERR = 0,
	LOG_WAR = 1,
	LOG_INF = 2,
	LOG_VER = 3,
	LOG_DBG = 4
} LogLevel;


/**
 * Log a message at the given level
 */
void logi(LogLevel level, char *tag, char *format, ...);

/**
 * Log an error message at the given level
 */
void loge(LogLevel level, char *tag, char *format, ...);
