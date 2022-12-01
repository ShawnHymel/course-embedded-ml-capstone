/**
 * Provide ei_printf() wrapper if we're not using the Edge Impulse SDK
 * 
 * Author: Shawn Hymel (Edge Impulse)
 * Date: September 9, 2022
 * License: Apache-2.0
 * 
 * Copyright 2022 EdgeImpulse, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PRINT_EMULATOR_H
#define PRINT_EMULATOR_H

#ifdef __cplusplus
extern "C" {
#endif

void ei_printf(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // PRINT_EMULATOR_H