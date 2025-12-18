#pragma once
#include <cstdarg>
namespace Eloquent {
    namespace ML {
        namespace Port {
            class DecisionTree {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        if (x[3] <= -0.4600013345479965) {
                            if (x[4] <= 0.4577720910310745) {
                                if (x[0] <= -0.8343954980373383) {
                                    if (x[7] <= -0.1984427273273468) {
                                        return 1;
                                    }

                                    else {
                                        return 0;
                                    }
                                }

                                else {
                                    if (x[4] <= 0.10541410371661186) {
                                        return 1;
                                    }

                                    else {
                                        if (x[7] <= -0.1984427273273468) {
                                            return 1;
                                        }

                                        else {
                                            return 0;
                                        }
                                    }
                                }
                            }

                            else {
                                return 0;
                            }
                        }

                        else {
                            if (x[7] <= -0.1984427273273468) {
                                if (x[4] <= 0.9214009940624237) {
                                    return 1;
                                }

                                else {
                                    return 0;
                                }
                            }

                            else {
                                if (x[0] <= 1.6401042342185974) {
                                    return 0;
                                }

                                else {
                                    return 1;
                                }
                            }
                        }
                    }

                protected:
                };
            }
        }
    }