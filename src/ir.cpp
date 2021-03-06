/*************************************************************************
	> File Name: ir.cpp
	> Author: 
	> Mail: 
	> Created Time: Thu 07 Mar 2019 07:59:28 AM UTC
 ************************************************************************/

#include "ir.h"
#include <memory>
#include "util.h"

namespace SC
{

VarExpr::VarExpr(DataType t, const std::string& label)
    : Expr(Variable::make(t, label))
{}

VarExpr::VarExpr(const std::string& label)
    : Expr(Variable::make(Int(32), label))
{}

VarExpr VarExpr::derive(const std::string& ext) const
{
    return VarExpr(get()->data_type, get()->label+ext);
}

Var Var::derive(const std::string& ext) const
{
    return Var(get()->data_type, get()->label+ext);
}
Expr Cast::make(DataType t, Expr v)
{
    CHECK_IF(v.notNull(), "Expr v is null");
    CHECK_IF(t.length() == v.type().length(), "length does not match!");
    Cast* n = new Cast();
    n->data_type = t;
    n->value = std::move(v);
    return n;
}

Expr And::make(Expr a, Expr b)
{
    CHECK_IF(a.notNull(), "Expr a is null");
    CHECK_IF(b.notNull(), "Expr a is null");
    CHECK_IF(a.type().isBool(), "Expr a is not bool");
    CHECK_IF(b.type().isBool(), "Expr b is not bool");
    And* n = new And();
    n->data_type = a.type();
    n->a = std::move(a);
    n->b = std::move(b);
    return n;
}

Expr Or::make(Expr a, Expr b)
{
    CHECK_IF(a.notNull(), "Expr a is null");
    CHECK_IF(b.notNull(), "Expr a is null");
    CHECK_IF(a.type().isBool(), "Expr a is not bool");
    CHECK_IF(b.type().isBool(), "Expr b is not bool");
    Or* n = new Or();
    n->data_type = a.type();
    n->a = std::move(a);
    n->b = std::move(b);
    return n;
}

Expr Not::make(Expr v)
{
    CHECK_IF(v.notNull(), "Expr v is null");
    CHECK_IF(v.type().isBool(), "Expr a is not bool");
    Not* n = new Not();
    n->data_type = v.type();
    n->value = std::move(n);
    return n;
}

Expr Variable::make(DataType t, const std::string& label)
{
    CHECK_IF(t.defined(), "type is not defined");
    Variable* n = new Variable();
    n->data_type = t;
    n->label = label;
    return n;
}

Expr Call::make(RetType rt,
        CallType ct, 
        const std::string& name, 
        std::vector<Expr>&& args,
        Function func)
{
    CHECK_IF(rt.defined(), "invalied rettype");
    CHECK_IF(name.length() > 0, "call must have a name");
    CHECK_IF(func.notNull(), "call to a null func");

    Call* n = new Call();
    n->data_type = rt;
    n->call_type = ct;
    n->name = name;
    n->args = std::move(args);
    n->func = std::move(func);
    return n;
}
Expr Call::make(RetType rt,
        CallType ct, 
        const std::string& name, 
        std::vector<Expr>&& args,
        TensorBase tb)
{
    CHECK_IF(rt.defined(), "invalied rettype");
    CHECK_IF(name.length() > 0, "call must have a name");
    CHECK_IF(tb.notNull(), "call to a null func");

    Call* n = new Call();
    n->data_type = rt;
    n->call_type = ct;
    n->name = name;
    n->args = std::move(args);
    n->tb = std::move(tb);
    return n;
}



Stmt LetStmt::make(VarExpr var, Expr value, Stmt body)
{
    CHECK_IF(var.notNull(), "iterator var is null");
    CHECK_IF(value.notNull(), "value is null");
    CHECK_IF(var.type()==value.type(), "type of var, value can not match");
    CHECK_IF(body.notNull(), "value is null");

    LetStmt* n = new LetStmt();
    n->var = std::move(var);
    n->value = std::move(value);
    n->body = std::move(body);
    return n;
}

Stmt Select::make(Expr cond, Expr true_case, Expr false_case)
{
    CHECK_IF(cond.notNull(), "cond is null");
    CHECK_IF(true_case.notNull(), "ture_case is null");
    CHECK_IF(false_case.notNull(), "false_case is null");
    CHECK_IF(cond.type().isBool(), "cond is not bool");
    CHECK_IF(true_case.type() == false_case.type(), "type of true,false cases can not match");

    Select* n = new Select();
    n->cond = std::move(cond);
    n->true_case = std::move(true_case);
    n->false_case = std::move(false_case);
    return n;
}

Stmt IfThenElse::make(Expr cond, Stmt then_case, Stmt else_case)
{
    CHECK_IF(cond.notNull(), "cond is null");
    CHECK_IF(then_case.notNull(), "then_case is null");
    CHECK_IF(cond.type().isBool(), "cond is not bool");

    IfThenElse* n = new IfThenElse();
    n->cond = std::move(cond);
    n->then_case = std::move(then_case);
    n->else_case = std::move(else_case);
    return n;
}

Stmt Block::make(Stmt first, Stmt rest)
{
    CHECK_IF(first.notNull(), "first is null");
    Block* n = new Block();
    n->first = std::move(first);
    n->rest = std::move(rest);
    return n;
}

Stmt For::make(ForType type, VarExpr var, 
        Expr min, Expr extent, Stmt body)
{
    CHECK_IF(var.notNull(), "var is null");
    CHECK_IF(min.notNull(), "min is null");
    CHECK_IF(extent.notNull(), "extent is null");
    CHECK_IF(body.notNull(), "body is null");

    CHECK_IF(min.type().isScalar(), "min is not scalar");
    // no need to check type of extent, we check the equality of type of min and extent.
    CHECK_IF(min.type() == extent.type(), "type of min, extent can not match");
    
    For* n = new For();
    n->for_type = type;
    n->var = std::move(var);
    n->min = std::move(min);
    n->extent = std::move(extent);
    n->body = std::move(body);
    return n;
}

Stmt Store::make(Expr lhs, Expr rhs)
{
    CHECK_IF(lhs.notNull() && rhs.notNull(), "operand of Store cannot be null");
    CHECK_IF(lhs.is_type<Variable>() || 
            (lhs.is_type<Call>() && lhs.cast_to<Call>()->call_type==CallType::TENSOR_ACCESS),
            "lhs mush be either variable or Tensor access");
    Store* n = new Store();
    n->lhs = std::move(lhs);
    n->rhs = std::move(rhs);
    return n;
}

Stmt Reduce::make(ReduceType reduce_type,
        Expr lhs, Expr rhs, std::vector<Iter> reduce_iters)
{
    CHECK_IF(lhs.notNull(), "lhs cannot be null");
    CHECK_IF(rhs.notNull(), "rhs cannot be null");
    for(auto& iter : reduce_iters)
    {
        CHECK_IF(iter->iter_type == IterType::REDUCTION, 
                "iter mush be reduction iter");
    }

    Reduce* n = new Reduce();
    n->reduce_type = reduce_type;
    n->lhs = std::move(lhs);
    n->rhs = std::move(rhs);
    n->reduce_iters = std::move(reduce_iters);
    return n;
}

} // namespace SC
