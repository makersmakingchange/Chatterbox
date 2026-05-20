<!--- Open Source Assistive Technology: GitHub Readme Template Version 1.2 (2024-May-27)  --->
 
<!--- TITLE --->
# Chatterbox

## Overview
<!--- A brief summary of the project. What it does, who it is for, how much it costs. --->
The Chatterbox is an Augmented and Alternative Communication (AAC) device that allows users to play pre-recorded messages using single or dual switch scanning. The Chatterbox provides auditory and visual feedback to indicate the message a user can play. 

The Chatterbox is comprised of off-the-shelf electronics, several custom printed circuit boards, and 3D printed parts. 

The Chatterbox is Open Assistive Technology (OpenAT) and is certified as Open Source Hardware by the Open Source Hardware Association under the OSHWA UID [CA000070](https://certification.oshwa.org/ca000070.html). Under the terms of the open source hardware licenses, the Chatterbox may be built, used, and improved by anyone.  

The overall cost of materials is about $135 CAD.

<img src="Photos/Chatterbox_Labels.png" width="500" alt="The Chatterbox with four labels printed paper labels to represent the messages that could be on each button. The Chatterbox is a green rectangle with four circular buttons, and the label holders are squares on the back at an angle. The printed labels are from left to right: the number one; a child raising their hand; a school bus; and three children singing.">

## Makers Making Change Assistive Device Library
https://www.makersmakingchange.com/product/chatterbox/01tJR00000CABPtYAP

## How to Obtain the Device
### 1. Do-it-Yourself (DIY) or Do-it-Together (DIT)

This is an open-source assistive technology, so anyone is free to build it. All of the files and instructions required to build the device are contained within this repository. Refer to the Maker Guide below.

### 2. Request a build of this device

You may also submit a build request through the [Makers Making Change Assistive Device Library Listing](https://www.makersmakingchange.com/product/chatterbox/01tJR00000CABPtYAP) to have a volunteer maker build the device. As the requestor, you are responsible for reimbursing the maker for the cost of materials and any shipping.

### 3. Build this device for someone else

If you have the skills and equipment to build this device, and would like to donate your time to create the device for someone who needs it, visit the [MMC Maker Wanted](https://makersmakingchange.com/maker-wanted/) section.


## Build Instructions
<!--- Outline the major steps required to create a build --->

### 1. Read through the Maker Guide

The [Maker Guide](/Documentation/Chatterbox_Maker_Guide.pdf)  contains all the necessary information to build this device, including tool lists, assembly instructions, programming instructions (if applicable) and testing.


### 2. Order the Off-The-Shelf Components

The [Bill of Materials](/Documentation/Chatterbox_BOM.csv) lists all of the parts and components required to build the device.

### 3. Order the custom PCBS

The [PCB Files](/Build_Files/PCB_Build_Files) contain the Gerber files needed to order the PCBs, and instructions on how to order them can be found in the [Maker Guide](/Documentation/Chatterbox_Maker_Guide.pdf).

### 4. Print the 3D Printable components

All of the files and individual print files can be found in the [/Build_Files/3D_Printing_Files](/Build_Files/3D_Printing_Files/) folder.

### 5. Assemble the Chatterbox

Reference the Assembly Guide section of the [Maker Guide](/Documentation/Chatterbox_Maker_Guide.pdf) for the tools and steps required to build and test the device.

## How to improve this Device
As open source assistive technology, you are welcomed and encouraged to improve upon the design. 

## Files
### Documentation
| Document             | Version | Link |
|----------------------|---------|------|
| Design Rationale     | 1.0     | [Chatterbox_Design_Rationale](/Documentation/Chatterbox_Design_Rationale.pdf)     |
| Maker Guide          | 1.0     | [Chatterbox_Maker_Guide](/Documentation/Chatterbox_Maker_Guide.pdf)     |
| Bill of Materials    | 1.0     | [Chatterbox_Bill_of_Materials](/Documentation/Chatterbox_BOM.xlsx)     |
| User Guide           | 1.0     | [Chatterbox_User_Guide](/Documentation/Chatterbox_User_Guide.pdf)    |
| Changelog            | 1.0     | [Changelog](CHANGES.txt)     |

### Design Files
<!--- Include a copy of the original design files to facilitate easy editing and customization. Consider also including a generic format (e.g., STEP) --->
 - [CAD Files](/Design_Files/CAD_Design_Files)
 - [PCB Files](/Design_Files/PCB_Design_Files)

### Build Files
<!--- Include a copy of the build files intended for manufacturing. This may include svg files for laser cutting, stl files for 3d printing, Gerber files for custom PCBs, and Arduino files for custom firmware. --->
 - [3D Printing Files](/Build_Files/3D_Printing_Files)
 - [PCB Build Files](/Build_Files/PCB_Build_Files)
 - [Firmware Files](/Build/Firmware_Files)


## License
<!--- Add the year(s) for the copyright and the Designer Name. You may use the standard set of open licenses or choose your own for the hardware, software, and accompanying materials. --->
Copyright (c) 2025-2026, Neil Squire.

This repository describes Open Hardware:
 - Everything needed or used to design, make, test, or prepare the Chatterbox is licensed under the [CERN 2.0 Weakly Reciprocal license (CERN-OHL-W v2) or later](https://cern.ch/cern-ohl ) .
 - All software is under the [GNU General Public License v3.0 (GPL-3.0)](https://www.gnu.org/licenses/gpl.html).
 - Accompanying material such as instruction manuals, videos, and other copyrightable works that are useful but not necessary to design, make, test, or prepare the Chatterbox are published under a [Creative Commons Attribution-ShareAlike 4.0 license (CC BY-SA 4.0)](https://creativecommons.org/licenses/by-sa/4.0/) .

You may redistribute and modify this documentation and make products using it under the terms of the [CERN-OHL-W v2](https://cern.ch/cern-ohl).
This documentation is distributed WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING OF MERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A PARTICULAR PURPOSE.
Please see the CERN-OHL-W v2 for applicable conditions.

Source Location: [Chatterbox Repository](https://github.com/makersmakingchange/Chatterbox)

## Attribution
<!--- Provide any necessary attribution for designs or components that are included in the device or as part of the project. --->
The device was designed by Stephan Dobri for the Neil Squire Society.

The Chatterbox is based on hardware and firmware from the [Open Playback Recorder](https://github.com/makersmakingchange/Open-Playback-Recorder).

<!--- This is the attribution for the template. --->
The documentation template was created by Makers Making Change / Neil Squire Society. It is available at the following link: [https://github.com/makersmakingchange/OpenAT-Template](https://github.com/makersmakingchange/OpenAT-Template)

### Contributors
<!--- List the names of the people that contributed to the design. This could include the original source of the idea, designers, testers, documenters, etc. --->
Designers:
 - Stephan Dobri
 - Jake McIvor

Feedback providers:
- [SpikedCola](https://github.com/SpikedCola)

Testers:
 - Surrey School AAC Team
 - SET-BC
 - Augmented and Alternative Communication - Interaction and Technology Lab (AAC-IT) at Queen's University

## Open Source Hardware Certification

The Chatterbox has been certified as open source hardware by the Open Source Hardware Association under the OSHWA UID [CA000070](https://certification.oshwa.org/ca000070.html).

<img src="oshwa/certification-mark-CA000070-stacked.png" width = 250 alt="Open Source Hardware Certification Mark for Shrub Hub: CA000070"> 
---

<!-- ABOUT MMC START -->
## About Makers Making Change
[<img src="https://raw.githubusercontent.com/makersmakingchange/makersmakingchange/main/img/mmc_logo.svg" width="500" alt="Makers Making Change Logo">](https://www.makersmakingchange.com/)

Makers Making Change is a program of [Neil Squire](https://www.neilsquire.ca/), a Canadian non-profit that uses technology, knowledge, and passion to empower people with disabilities.

Makers Making Change leverages the capacity of community based Makers, Disability Professionals and Volunteers to develop and deliver affordable Open Source Assistive Technologies.

 - Website: [www.MakersMakingChange.com](https://www.makersmakingchange.com/)
 - GitHub: [makersmakingchange](https://github.com/makersmakingchange)
 - Bluesky: [@makersmakingchange.bsky.social](https://bsky.app/profile/makersmakingchange.bsky.social)
 - Instagram: [@makersmakingchange](https://www.instagram.com/makersmakingchange)
 - Facebook: [makersmakechange](https://www.facebook.com/makersmakechange)
 - LinkedIn: [Neil Squire Society](https://www.linkedin.com/company/neil-squire-society/)
 - Thingiverse: [makersmakingchange](https://www.thingiverse.com/makersmakingchange/about)
 - Printables: [MakersMakingChange](https://www.printables.com/@MakersMakingChange)

### Contact Us
For technical questions, to get involved, or to share your experience we encourage you to [visit our website](https://www.makersmakingchange.com/) or [contact us](https://www.makersmakingchange.com/s/contact).
<!-- ABOUT MMC END -->
