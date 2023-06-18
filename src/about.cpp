// SPDX-License-Identifier: GPL-2.0-or-later
// PDX-FileCopyrightText: 2023 Denys Madureira <denysmb@zoho.com>

#include "about.h"

KAboutData AboutType::aboutData() const
{
    return KAboutData::applicationData();
}
