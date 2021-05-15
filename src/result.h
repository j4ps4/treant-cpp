#include <variant>

template<typename ResT, typename ErrT>
class Result
{
private:
    struct ErrWrap
    {
        ErrWrap(ErrT&& err) : err_(std::forward(err)) {}
        ErrT err_;
    };
    Result(std::variant<ResT, ErrWrap>&& var) : data_(std::forward(var)) {}
public:
    Result(ResT&& res) : data_(std::forward(res)) {}
    // Result(ErrT&& err) : data_(std::forward(err)) {}
    Result(const ResT& res) : data_(std::forward(res)) {}
    // Result(const ErrT& err) : data_(std::forward(err)) {}

    friend Result<ResT, ErrT> error(ErrT1&& err)
    {
        std::variant<ResT, ErrWrap> var = std::forward(err);
        return Result(std::move(var));
    }

    bool is_err() const
    {
        return std::get_if<ErrWrap>(&data_) != nullptr;
    }
    bool is_ok() const
    {
        return std::get_if<ErrWrap>(&data_) == nullptr;
    }
    ErrT& err()
    {
        return std::get<ErrWrap>(data_).err_;
    }
    ResT& result()
    {
        return std::get<ResT>(data_);
    }
private:
    std::variant<ResT, ErrWrap> data_;
};