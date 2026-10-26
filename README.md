# MLPR-system: An Efficient Real-Time Moroccan Automatic License Plate Recognition System Based on the YOLO Object Detector

- This repo is for implementing MLPR from this paper: https://link.springer.com/chapter/10.1007/978-3-031-28387-1_25
- The ALPR system block diagram:
![1(1)](https://github.com/ZainabOuardirhi/MLPR-system/assets/81168343/c9de005e-517a-4bfb-8794-7a36fc5cf322)

## Published results

The associated paper reports the following results from its evaluated datasets:

| Evaluation | Reported result |
| --- | --- |
| Full system — license-plate detection | **98.47% recall and 98.47% precision** (450 correctly detected plates from 457 positive vehicle images; 7 false positives) |
| Full system — complete plate recognition | **91.11%** (410 of 450 detected plates recognized with all characters correct) |
| Full system — at most one incorrect character | **98.89%** (445 of 450 detected plates, including 35 with one character error) |
| Separate character-recognition experiment | **95.62%** (2,336 of 2,443 Moroccan and European plate images completely recognized) |

These numbers refer to **different evaluations in the publication** and should not be treated as interchangeable. The paper also reports **97% accuracy, 98% recall and 97% F1-score** in its ablation comparison for the full MALPR approach with plate straightening and voting (Table 5).

The system combines YOLOv3-based plate and character detection with a zone of interest, tracking, plate straightening and voting across frames. Results are reproduced **from the publication**, not from a new execution of this repository.

## Publication

**Z. Ouardirhi et al.**, *An Efficient Real-Time Moroccan Automatic License Plate Recognition System Based on the YOLO Object Detector*. In **Proceedings of the 6th International Conference on Big Data and Internet of Things (BDIoT 2022)**, Lecture Notes in Networks and Systems, vol. 625, pp. 290–302, Springer (published online 2023).

- [Published chapter (DOI)](https://doi.org/10.1007/978-3-031-28387-1_25)
- [Author manuscript and publication record (UMONS)](https://orbi.umons.ac.be/handle/20.500.12907/45005)

*The experimental images and trained network weights described in the paper are not included in this code repository.*
