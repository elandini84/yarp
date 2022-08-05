/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MANAGER_ARBITRATOR
#define YARP_MANAGER_ARBITRATOR

#include <map>
#include <vector>

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>
#include <yarp/manager/utility.h>


namespace yarp::manager {


/**
 * Class port Arbitrator
 */

class Arbitrator {
public:
    Arbitrator() = default;
    Arbitrator(const char* szPort) : strPort(szPort) {}
    ~Arbitrator() = default;

    void setPort(const char* szPort) { if(szPort) { strPort = szPort; } }
    void updateStringsParams(const std::map<std::string,std::string>& parameters) { strPort.updateParameters(parameters); }

    const char* getPort() { return strPort.str().c_str(); }

    const char* getRule(const char* con)
    {
        if(con && (rules.find(con) != rules.end())) {
            return rules[con].str().c_str();
        }
        return nullptr;
    }

    std::map<std::string, ParamStrStream>& getRuleMap() { return rules; }
    int ruleCount() { return rules.size(); }

    GraphicModel* getModel() { return model;}
    void setModel(GraphicModel* mdl) { model = mdl; };

    // modelBased is used to keep the graphic and geometric
    // information which is directly loaded from application
    // description file.
    GraphicModel& getModelBase() { return modelBase;}
    void setModelBase(GraphicModel& mdl) { modelBase = mdl; };

    void setOwner(Node* owner){ appOwner = owner; }
    Node* owner() { return appOwner; }

    bool operator==(const Arbitrator& alt) const {
        return (strPort.str() == alt.strPort.str());
    }

    std::map<std::string, double>& getAlphas( const char* con) {
        return alphas[con];
    }

    double getBias(const char* con) {
        return biases[con];
    }

    void addRule(const char* con, const char* rule);
    void removeRule(const char* con);
    bool trainWeights(const char* opnd);
    bool trainWeights();
    bool validate();

protected:

private:
    ParamStrStream strPort;
    std::map<std::string, ParamStrStream> rules;
    GraphicModel* model {nullptr};
    GraphicModel modelBase;
    Node* appOwner {nullptr};

    std::map<std::string, std::map<std::string, double> > alphas;
    std::map<std::string, double> biases;

};


typedef std::vector<Arbitrator> ArbContainer;
typedef std::vector<Arbitrator>::iterator ArbIterator;

} // namespace yarp::manager


#endif // __YARP_MANAGER_ARBITRATOR__
