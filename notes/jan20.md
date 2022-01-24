## 1/20/22 Notes
### Virtual Memory Map
- Software refers to mem and SFR using virtual Address
- PA = VA && 0x1FFFFFFF
- First three bits are used to designate what is cachable or not
    - Cached data can be faster for CPU prefetch
    - Cached is not the latest data but is still useful
- 