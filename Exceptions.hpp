#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

/*!
 * \brief The FileOpeningErrorException class
 */
class FileOpeningErrorException : std::exception {

public:
    FileOpeningErrorException(const char * p) : place(p){}
    const char* what() const noexcept { return (std::string("FileOpeningErrorException: ") + place).c_str(); }

private:
    const std::string place;
};

/*!
 * \brief The UnexpectedFileEndException class
 */
class UnexpectedFileEndException : std::exception {

public:
    UnexpectedFileEndException(const char * p) : place(p){}
    const char* what() const noexcept { return (std::string("UnexpectedFileEndException: ") + place).c_str(); }

private:
     const std::string place;
};

/*!
 * \brief The FileNotExistsException class
 */
class FileNotExistsException : std::exception {

public:
    FileNotExistsException(const char * p) : place(p){}
    const char* what() const noexcept { return (std::string("FileNotExistsException: ") + place).c_str(); }

private:
     const std::string place;
};

#endif // EXCEPTIONS_HPP
