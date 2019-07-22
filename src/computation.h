/*************************************************************************
	> File Name: computation.h
	> Author: 
	> Mail: 
	> Created Time: Mon 11 Mar 2019 06:40:45 AM UTC
 ************************************************************************/

#ifndef _COMPUTATION_H
#define _COMPUTATION_H

#include "function.h"
#include "iter.h"
#include "ir.h"
#include "tensor.h"
#include <vector>


namespace SC
{

class Computation;
class Schedule;


/**
 * \bref computation refers to a standalone computation block, 
 * like loop-based computing code.
 */
class ComputationNode : public FunctionNode
{
public:
    virtual void calcu_input_tensors() = 0;
    virtual void calcu_output_tensors() = 0;
    ComputationNode(FunctionNodeType type) : FunctionNode(type) {}

    std::vector<Tensor> inputTensors;
    std::vector<Tensor> outputTensors;

    virtual ~ComputationNode() {}
    /**
     * \bref root iters represents the outer most loops
     */
    virtual const std::vector<Iter>& rootIters() const = 0; 
    virtual Stmt buildBody() const = 0;

    const std::vector<Tensor>& input_tensors() const {

        return inputTensors;
    }
    const std::vector<Tensor>& output_tensors() const {
        return outputTensors;
    }
};

/**
 * \bref reference of computation node
 */
/*class Computation : public Function
{
public:
    using ContainerType = ComputationNode;
    Computation() : Function() {}
    Computation(const FunctionNode* p) : Function(p) {}

    const ComputationNode* get() const { return (const ComputationNode*)ptr; }
    const ComputationNode* operator->() const { return get(); }
    const ComputationNode& operator*() const { return *get(); }

};*/

template <typename T>
class CPNode : public ComputationNode
{
public:
    CPNode() : ComputationNode(T::_node_type) {}
    virtual ~CPNode() {}
};


class PlaceHolderComNode : public CPNode<PlaceHolderComNode>
{

public:
    std::vector<Expr> shape;
    DataType data_type;
    std::vector<Iter> root_iters;
    static const FunctionNodeType _node_type = FunctionNodeType::PLACEHOLDER;

    void calcu_input_tensors() override {}
    void calcu_output_tensors() override;

    const std::vector<Iter>& rootIters() const override { 
        return root_iters; };
    Stmt buildBody() const override { return Stmt(); }

    static Computation make(const std::string& name,
            std::vector<Expr> shape,
            DataType data_type);

};

class AllocateComNode : public CPNode<AllocateComNode>
{
public:
    std::vector<Expr> shape;
    DataType data_type;
    std::vector<Iter> root_iters;
    TensorLoc loc;
    static const FunctionNodeType _node_type = FunctionNodeType::ALLOCATE;

    void calcu_input_tensors() override {}
    void calcu_output_tensors() override;

    const std::vector<Iter>& rootIters() const override { 
        return root_iters; };
    Stmt buildBody() const override;

    static Computation make(const std::string& name,
            std::vector<Expr> shape,
            DataType data_type,
            TensorLoc loc=TensorLoc::MEM);
};


/**
 * \bref nest loop computations
 */
class NestLoopComNode : public CPNode<NestLoopComNode>
{
protected:
    /**
     * \bref calcu input tensors, and set the source_computation
     * of the tensors
     */
    void calcu_input_tensors() override;
    /**
     * \bref calcu output tensors, and set the source_computation
     * of the tensors
     */
    void calcu_output_tensors() override;

public:
    /**
     * \bref label
     */
    //std::string label;
    /**
     * \bref root iters represent the outer most loops
     */
    std::vector<Iter> root_iters;

    /**
     * \bref reduction iters in body
     */
    std::vector<Iter> reduce_iters;

    /**
     * \bref body, body is not a stmt, but {Expr ...}.
     * it will be lowered to multiple statements
     */
    //std::vector<Expr> body;
    
    /**
     * bref body
     */
    std::vector<Stmt> body;
    //Stmt body;


    static const FunctionNodeType _node_type = FunctionNodeType::NEST_LOOP;

    const std::vector<Iter>& rootIters() const override{ return root_iters; }

    Stmt buildBody() const override;

    static Computation make(Schedule& s,
            const std::string& name, 
            std::vector<Iter> root_iters, 
            std::vector<Iter> reduce_iters,
            std::vector<Stmt> body);
    static Computation make(Schedule& s,
            const std::string& name,
            std::vector<Iter> root_iters,
            std::vector<Stmt> body);
};

/**
 * \bref lhs += rhs
 */
inline Stmt reduce_add(Expr lhs, Expr rhs)
{
    return Reduce::make(ReduceType::ADD, std::move(lhs), std::move(rhs), {});
}

/**
 * \bref nest loop computation
 */
inline Computation nest_loop_computation(
        Schedule& s, 
        const std::string& name,
        std::vector<Iter> root_iters,
        std::vector<Iter> reduce_iters,
        std::vector<Stmt> body)
{
    return NestLoopComNode::make(s,
            name, std::move(root_iters), 
            std::move(reduce_iters), 
            std::move(body));
}

/**
 * \bref nest loop computation without reduce_iters
 */
inline Computation nest_loop_computation(
        Schedule& s,
        const std::string& name,
        std::vector<Iter> root_iters,
        std::vector<Stmt> body)
{
    return NestLoopComNode::make(s,
            name, std::move(root_iters), 
            {},
            std::move(body));
}

} // namespace SC


#endif
