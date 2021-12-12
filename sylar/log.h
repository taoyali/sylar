/**
 * @file log.h
 * @author taoyali (1312315229@qq.com)
 * @brief 日志模块封装
 * @version 0.1
 * @date 2021-12-08
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <stdarg.h>
#include <stdint.h>

#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief 使用流式方式将日志级别level的日志写入到logger
 *
 */
#define SYLAR_LOG_LEVEL(logger, level)                                \
  if (logger->getLevel() <= level)                                    \
  sylar::LogEventWrap(                                                \
      sylar::LogEvent::ptr(new sylar::LogEvent(                       \
          logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId(), \
          sylar::GetFibreId(), time(0), sylar::Thread::GetName())))   \
      .getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

/**
 * @brief 使用格式化方式将日志级别level的日志写入到logger
 *
 */
#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...)                  \
  if (logger->getLevel() <= level)                                    \
  sylar::LogEventWrap(                                                \
      sylar::LogEvent::ptr(new sylar::LogEvent(                       \
          logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId(), \
          sylar::GetFirbeId(), time(0), sylar::Thread::GetName())))   \
      .getEvent()                                                     \
      ->format(fmt, __VA_ARGS__)

#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) \
  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) \
  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) \
  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) \
  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) \
  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

/**
 * @brief 获取主日志器
 *
 */
#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstall()->getRoot()
/**
 * @brief 获取name日志器
 *
 */
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstall()->getLogger(name)

namespace sylar {

class Logger;
class LoggerManager;

/**
 * @brief 日志级别
 *
 */
class LogLevel {
 public:
  /**
   * @brief 日志级别枚举
   *
   */
  enum Level {
    UNKNOW = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
  };

  /**
   * @brief 将日志级别转成文本输出
   *
   * @param level 日志级别
   * @return const char*
   */
  static const char* ToString(LogLevel::Level level);
  /**
   * @brief 将文本转换成日志级别
   *
   * @param str 日志级别文本
   * @return LogLevel::Level
   */
  static LogLevel::Level FromString(const std::string& str);
};

/**
 * @brief 日志事件
 *
 */
class LogEvent {
 public:
  typedef std::shared_ptr<LogEvent> ptr;

  /**
   * @brief Construct a new Log Event object  构造函数
   *
   * @param logger	日志器
   * @param level 	日志级别
   * @param file 		文件名
   * @param line 		文件行号
   * @param elapse 	程序启动依赖的耗时(毫秒)
   * @param thread_id 线程id
   * @param fiber_id 	协程id
   * @param time 		日志时间(秒)
   * @param thread_name 线程名称
   */
  LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
           const char* file, int32_t line, uint32_t elapse, uint32_t thread_id,
           uint32_t fiber_id, uint64_t time, const std::string& thread_name);

  const char* getFile() const { return m_file; }
  int32_t getLine() const { return m_line; }
  uint32_t getElapse() const { return m_elapse; }
  uint32_t getThread() const { return m_threadId; }
  uint32_t getFiberId() const { return m_fiberId; }
  uint32_t getTime() const { return m_time; }
  const std::string& getThreadName() const { return m_threadName; }
  std::string getContent() const { return m_ss.str(); }
  std::shared_ptr<Logger> getLogger() const { return m_logger; }
  LogLevel::Level getLevel() const { return m_level; }
  std::stringstream& getSS() { return m_ss; }

  /**
   * @brief 格式化写入日志内容
   *
   * @param fmt
   * @param ...
   */
  void format(const char* fmt, ...);

  /**
   * @brief 格式化写入日志内容
   *
   * @param fmt
   * @param al
   */
  void format(const char* fmt, va_list al);

 private:
  const char* m_file = nullptr;      // 文件名`
  int32_t m_line = 0;                // 行号
  uint32_t m_elapse = 0;             // 程序启动开始到现在的毫秒数
  uint32_t m_threadId = 0;           // 线程号
  uint32_t m_fiberId = 0;            // 协程号
  uint64_t m_time;                   // 时间戳
  std::string m_threadName;          // 线程名称
  std::string m_ss;                  // 日志内容流
  std::shared_ptr<Logger> m_logger;  // 日志器
  LogLevel::Level m_level;           // 日志级别
};

class LogEventWrap {
 public:
  /**
   * @brief Construct a new Log Event Wrap object 构造函数
   *
   * @param e 日志事件
   */
  LogEventWrap(LogEvent::ptr e);

  /**
   * @brief Destroy the Log Event Wrap object 析构函数
   *
   */
  ~LogEventWrap();
  /**
   * @brief Get the Event object 获取日志事件
   *
   * @return LogEvent::ptr 日志事件
   */
  LogEvent::ptr getEvent() const { return m_event; }
  /**
   * @brief 获取日志内容流
   *
   * @return std::stringstream&
   */

  std::stringstream& getSS();

 private:
  /**
   * @brief 日志事件
   *
   */
  LogEvent::ptr m_event;
}

// 日志格式器
class LogFormatter {
 public:
  typedef std::shared_ptr<LogFormatter> ptr;
  /**
   * @brief Construct a new Log Formatter object 构造函数
   *
   * @param pattern 格式模板
   *
   * @details
   *  %m 消息
   *  %p 日志级别
   *  %r 累计毫秒数
   *  %c 日志名称
   *  %t 线程id
   *  %n 换行
   *  %d 时间
   *  %f 文件名
   *  %l 行号
   *  %T 制表符
   *  %F 协程id
   *  %N 线程名称
   *
   *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
   */
  LogFormatter(const std::string& pattern);

  /**
   * @brief 返回格式化日志文本
   *
   * @param logger 日志器
   * @param level 日志级别
   * @param event 日志事件
   * @return std::string
   */
  std::string format(Logger::ptr logger, LogLevel::Level level,
                     LogEvent::ptr event);
  std::ostream& format(std::ostream& ofs, Logger::ptr logger,
                       LogLevel::Level level, LogEvent::ptr event);

 public:
  /**
   * @brief 日志内容格式化
   *
   */
  class FormatItem {
   public:
    typedef std::shared_ptr<FormatItem> ptr;
    virtual ~FormatItem(){};
    /**
     * @brief 格式化日志到流
     *
     * @param os 日志输出流
     * @param logger 日志器
     * @param level 日志等级
     * @param event 日志事件
     */
    virtual void format(std::ostream& os, Logger::ptr logger,
                        LogLevel::Level level, LogEvent::ptr event) = 0;
  };

  /**
   * @brief 初始化,解析日志模板
   *
   */
  void init();
  /**
   * @brief 是否有错误
   *
   * @return true
   * @return false
   */
  bool isError() const { return m_error; }

  /**
   * @brief Get the Pattern object 返回日志模板
   *
   * @return const std::string
   */
  const std::string getPattern() const { return m_pattern; }

 private:
  /// 日志格式模板
  std::string m_pattern;
  /// 日志格式解析后格式
  std::vector<FormatItem::ptr> m_items;
  /// 是否有错误
  bool m_error = false;
};

// 日志输出目标
class LogAppender {
  friend class Logger;

 public:
  typedef std::shared_ptr<LogAppender> ptr;
  /**
   * @brief Destroy the Log Appender object 析构函数
   *
   */
  virtual ~LogAppender() {}
  /**
   * @brief 写入日志
   *
   * @param logger 日志器
   * @param level 日志级别
   * @param event 日志事件
   */
  virtual void log(Logger::ptr logger, LogLevel::Level level,
                   LogEvent::ptr event) = 0;

  /**
   * @brief 将日志输出目标配置转成YAML String
   *
   */
  void std::string toYamlString() = 0;

  /**
   * @brief Set the Formatter object 更改日志格式器
   *
   * @param val
   */
  void setFormatter(LogFormatter::ptr val);

  /**
   * @brief Get the Fommater object 获取日志格式器
   *
   * @return LogFormatter::ptr
   */
  LogFormatter::ptr getFommater();

  LogLevel::Level getLevel() const { return m_level; }

  void setLevel(LogLevel::Level level) { m_level = level; }

  // void setFormatter(LogFormatter::ptr val) { m_formatter = val; }
  // LogFormatter::ptr getFormatter() const { return m_formatter; }

 private:
  LogLevel::Level m_level = LogLevel::DEBUG;
  bool m_hasFormatter = false;
  MutexType m_mutex;
  LogFormatter::ptr m_formatter;
};

/**
 * @brief 日志器
 *
 */
class Logger : public std::enable_shared_from_this<Logger> {
  friend class LoggerManager;

 public:
  typedef std::shared_ptr<Logger> ptr;
  typedef Spinlock MutexType;

  /**
   * @brief Construct a new Logger object 析构函数
   *
   * @param name 日志器名称
   */
  Logger(const std::string& name = "root");

  /**
   * @brief 写日志
   *
   * @param level 日志级别
   * @param event 日志事件
   */
  void log(LogLevel::Level level, LogEvent::ptr event);
  /**
   * @brief 写debug级别日志
   *
   * @param event
   */
  void debug(LogEvent::ptr event);
  void info(LogEvent::ptr event);
  void warn(LogEvent::ptr event);
  void fatal(LogEvent::ptr event);
  /**
   * @brief 添加日志目标
   *
   * @param appender
   */
  void addAppender(LogAppender::ptr appender);
  /**
   * @brief 删除日志目标
   *
   * @param appender
   */
  void delAppender(LogAppender::ptr appender);
  /**
   * @brief 清空日志目标
   *
   */
  void clearAppenders();
  /**
   * @brief Get the Name object 获取日志名称
   *
   * @return const std::string&
   */
  const std::string& getName() const { return m_name; }

  void setLevel(LogLevel::Level level) { m_level = level; }
  LogLevel::Level getLevel() { return m_level; }

  void setFormatter(LogFormatter::ptr var);
  /**
   * @brief Set the Formatter object
   *
   * @param val
   */
  LogFormatter::ptr getFormatter();
  /**
   * @brief 将日志器的配置转成YAML String
   *
   * @return std::string
   */
  std::string toYamlString();

 private:
  std::string m_name;                       // 日志名称
  LogLevel::Level m_level;                  // 日志级别
  MutexType m_mutex;                        // mutex
  std::list<LogAppender::ptr> m_appenders;  // Appender集合
  LogFormatter::ptr m_formatter;            // 日志器格式
  Logger::ptr m_root;                       // 主日志器
};

//输出到控制台的Appender
class StdoutLogAppender : public LogAppender {
 public:
  typedef std::shared_ptr<StdoutLogAppender> ptr;
  void log(Logger::ptr logger, LogLevel::Level level,
           LogEvent::ptr event) override;

  std::string toYamlString() override;
};

//输出到文件
class FileLogAppender : public LogAppender {
 public:
  typedef std::shared_ptr<FileLogAppender> ptr;
  FileLogAppender(const std::string& filename);
  void log(Logger::ptr logger, LogLevel::Level level,
           LogEvent::ptr event) override;

  // 重新打开文件， 文件打开成功返回true
  bool reopen();

 private:
  std::string m_filename;
  std::ofstream m_filestream;
  /// 上次重新打开时间
  uint64_t m_lastTime = 0;
};

class LoggerManager {
 public:
  typedef Spinlock MutexType;
  LoggerManager();

  Logger::ptr getLogger(const std::string& name);

  void init();

  Logger::ptr getRoot() const { return m_root; }

  std::string toYamlString();

 private:
  MutexType m_mutex;
  std::map<std::string, Logger::ptr> m_logger;
  Logger::ptr m_root;
};

/// 日志管理类单例模式
typedef sylar::Singleton<LoggerManager> LoggerMgr;

}  // namespace sylar

#endif
