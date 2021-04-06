#include "stdafx.h"
#include "Error.h"

namespace PlayFab
{

HRESULT ServiceErrorToHR(ServiceErrorCode errorCode)
{
    // TODO define appropriate HRs for PlayFab service errors. Probably want to bucket them or translate
    // to generic PlayFab service error hr. Just returning E_FAIL for now.
    UNREFERENCED_PARAMETER(errorCode);
    return E_FAIL;
}

HRESULT CurrentExceptionToHR()
{
    try
    {
        throw;
    }
    // std exceptions
    catch (const std::bad_alloc&) // is an exception
    {
        return E_OUTOFMEMORY;
    }
    catch (const std::bad_cast&) // is an exception
    {
        return E_NOINTERFACE;
    }
    catch (const std::invalid_argument&) // is a logic_error
    {
        return E_INVALIDARG;
    }
    catch (const std::out_of_range&) // is a logic_error
    {
        return E_BOUNDS;
    }
    catch (const std::length_error&) // is a logic_error
    {
        return __HRESULT_FROM_WIN32(ERROR_BAD_LENGTH);
    }
    catch (const std::overflow_error&) // is a runtime_error
    {
        return __HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
    }
    catch (const std::underflow_error&) // is a runtime_error
    {
        return __HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
    }
    catch (const std::range_error&) // is a runtime_error
    {
        return E_BOUNDS;
    }
    catch (const std::system_error& ex) // is a runtime_error
    {
        if (ex.code().category() == std::system_category())
        {
            return __HRESULT_FROM_WIN32(ex.code().value());
        }
        else
        {
            return ex.code().value();
        }
    }
    catch (const std::logic_error&) // is an exception
    {
        return E_UNEXPECTED;
    }
    catch (const std::runtime_error&) // is an exception
    {
        return E_FAIL;
    }
    catch (const std::exception&) // base class for standard C++ exceptions
    {
        return E_FAIL;
    }
    catch (HRESULT exceptionHR)
    {
        return exceptionHR;
    }
    catch (...) // everything else
    {
        return E_FAIL;
    }
}

}
