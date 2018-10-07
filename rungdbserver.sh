#!/bin/bash
JLinkGDBServer -select USB -device STM32F411RE -endian little -if SWD -speed 4000 -noir -LocalHostOnly
