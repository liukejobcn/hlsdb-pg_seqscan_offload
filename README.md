# hlsdb-pg_seqscan_offload

FPGA offloading of tuple parsing and predicate filtering for PostgreSQL sequential scans.

This repository contains the research code and experimental artifacts accompanying:

> Ke Liu, Haonan Tong, Zhongxiang Sun, Zhixin Ren, Guangkui Huang, Hongyin Zhu,
> Luyang Liu, Qunyang Lin, and Chuang Zhang.
> **Integrating FPGA-based hardware acceleration with relational databases.**
> *Parallel Computing* 119 (2024), 103064.
> [https://doi.org/10.1016/j.parco.2024.103064](https://doi.org/10.1016/j.parco.2024.103064)

Ke Liu and Haonan Tong contributed equally to the paper.

## Status and provenance

This project is primarily preserved as a **paper code archive**. The GitHub documentation
refresh describes the original artifact; it does not represent a new FPGA test run or a
claim of compatibility with current PostgreSQL, Linux, Vitis, or FPGA platforms.
Performance results in the paper and `test/` belong to the original experiments.

- **GitHub:** [liukejob/hlsdb-pg_seqscan_offload](https://github.com/liukejob/hlsdb-pg_seqscan_offload)
- **Original Gitee repository:** [liukejob/hlsdb-pg_seqscan_offload](https://gitee.com/liukejob/hlsdb-pg_seqscan_offload)
- **Imported source baseline:** `2f06980860144398887c3658f06cf68f64870257` (2023-12-18), with 44 original commits.
- **Branches:** `master` preserves that original source snapshot; `main` adds the GitHub-facing documentation and citation metadata.

The Gitee repository remains in place because the published paper links to it. The
GitHub copy retains the original Git history and existing license notices.

## What the system does

The implementation connects three components:

1. A **PostgreSQL hook module** extracts tuple-layout and predicate information and
   installs a custom scan path.
2. A **host middleware process** schedules transfers and FPGA work through Xilinx XRT/OpenCL.
3. **FPGA kernels** parse PostgreSQL tuples and filter rows using a pipelined design.

The paper describes an end-to-end system that retains native SQL and the PostgreSQL
storage format. The artifact focuses on sequential-scan offloading; the presence of
TPC-H query files does not mean every query or PostgreSQL operator is accelerated.

## Repository map

| Path | Contents |
| --- | --- |
| [`src/hook/pg_seqscan_hook_v2.0/`](src/hook/pg_seqscan_hook_v2.0/) | PostgreSQL hook and custom scan implementation |
| [`src/middleware-and-kernel/pg_seqscan_v3.5.4/`](src/middleware-and-kernel/pg_seqscan_v3.5.4/) | Host middleware, FPGA source, Makefiles, and platform configuration |
| [`bin/`](bin/) | Original v3.5.4 binary archive, including a platform-specific `.xclbin` |
| [`tools/`](tools/) | Dataset preparation, SQL files, benchmark launchers, and result parsers |
| [`test/`](test/) | Original spreadsheets and archived experiment logs |
| [`doc/`](doc/) | Original setup documents and reproduction notes |

The historical directory spelling `test/pereformance-En/` is retained to preserve
existing paths. Its subdirectories associate archived measurements with paper figures.

## Reading and reproducing the artifact

Start with [reproduction notes and known documentation discrepancies](doc/reproducibility.md).
They explain historical platform assumptions, the actual binary archive name,
and the data-changing behavior of the dataset setup script.

The original instructions are retained for reference:

- [Original English setup guide, preserved unchanged](doc/legacy-setup-en.md)
- [Original Chinese setup guide](doc/README-CN.md)
- [Original environment and installation notes](doc/01-%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA%E4%B8%8E%E5%AE%89%E8%A3%85.md)

The documented historical software baseline is PostgreSQL **11.2**, Ubuntu **18.04 x64**,
Linux **5.0.0-23-generic**, and Vitis **2020.2**, with an installed XRT runtime.
The source defaults to the `inspur_f37x_xdma_201920_3` platform and an eight-compute-unit
HBM configuration. A different board or platform requires its own configuration;
the archived bitstream is not a generic image for arbitrary FPGA cards.

No compilation, database execution, FPGA loading, or performance reproduction was
performed as part of this documentation refresh. Source inspection and archive
inventory checks are described separately from runtime validation in the reproduction notes.

## Citation

If this artifact supports your research, cite the paper above. Machine-readable
citation metadata is provided in [`CITATION.cff`](CITATION.cff).

```bibtex
@article{liu2024integrating,
  title   = {Integrating FPGA-based hardware acceleration with relational databases},
  author  = {Liu, Ke and Tong, Haonan and Sun, Zhongxiang and Ren, Zhixin and Huang, Guangkui and Zhu, Hongyin and Liu, Luyang and Lin, Qunyang and Zhang, Chuang},
  journal = {Parallel Computing},
  volume  = {119},
  pages   = {103064},
  year    = {2024},
  doi     = {10.1016/j.parco.2024.103064}
}
```

## License

The repository carries the GNU Affero General Public License, version 3, in
[`LICENSE`](LICENSE). Existing third-party copyright and license notices remain in
place; for example, the Xilinx `xcl2` helper source includes Apache-2.0 notices.
The GitHub migration does not relicense the code or those third-party components.
