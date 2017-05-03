// Copyright 2017 Lybros.

#ifndef SRC_THEIA_UTILS_H_
#define SRC_THEIA_UTILS_H_

#include <QString>

#include "theia/theia.h"

using theia::DescriptorExtractorType;
using theia::MatchingStrategy;

QString DescriptorExtractorTypeToString(DescriptorExtractorType);

DescriptorExtractorType DescriptorExtractorTypeFromString(QString);

QString MatchingStrategyToString(MatchingStrategy);

MatchingStrategy MatchingStrategyFromString(QString);

#endif  // SRC_THEIA_UTILS_H_
