# TearedEQ

An 8-band parametric EQ (VST3) with a live spectrum analyzer and a draggable EQ curve.

- 8 bands, each with: **Bell, Low Shelf, High Shelf, Low Cut, High Cut, Notch**
- Frequency 20 Hz - 20 kHz, Gain +/-18 dB, Q 0.1 - 18
- Output gain, real-time spectrum analyzer (toggle in the top-right)
- Drag nodes on the graph - **scroll wheel on a node = Q** - **double-click empty space = add a band**, double-click a node = remove it

You do **not** install anything on your PC. GitHub builds the plugin for you in the cloud.

---

## How to get the plugin (about 5 minutes)

1. **Unzip** this zip somewhere on your PC.
2. Go to **github.com** and log in (free account is fine).
   Click **+** (top right) -> **New repository**. Name it `TearedEQ`, choose **Private**, click **Create repository**.
3. On the new empty repository page click **"uploading an existing file"**.
   Open the unzipped folder, select **everything inside it** (the `.github` folder, `Source` folder, `CMakeLists.txt`, `README.md`, `.gitignore`) and **drag it all into the browser window**.
   Scroll down and click **Commit changes**.
   - Make sure the `.github` folder came along. If you can't see it in Explorer, turn on *View -> Show -> Hidden items*.
   - Fallback if `.github` didn't upload: in the repo click **Add file -> Create new file**, type `.github/workflows/build.yml` as the name (the slashes create the folders), paste the contents of that file, and commit.
4. Click the **Actions** tab. A build called **"Build TearedEQ (Windows VST3)"** starts automatically
   (if Actions asks you to enable workflows, click the green button). Wait roughly 5-10 minutes for a green check mark.
   (You can also start it manually: Actions -> the workflow -> **Run workflow**.)
5. Click the finished run, scroll to **Artifacts** at the bottom, and download **TearedEQ-VST3-Windows**.
6. Unzip it. You get a folder called **`TearedEQ.vst3`**.
   Copy that whole folder to:

   `C:\Program Files\Common Files\VST3`

   (Windows will ask for administrator permission - click Continue.)
7. Open **FL Studio** -> **Options -> Manage plugins** -> **Find installed plugins** (make sure "Verify plugins" is on if you like) -> **Start scan**.
   Then add **TearedEQ** to a mixer insert slot (it appears under *Installed -> Effects*, or search "Teared").

If a build ever fails (red X), open the failed run, click the failed step, and copy the error text to get it fixed.

---

## Notes

- Windows 64-bit VST3 only. The C++ runtime is linked statically, so no extra redistributables are needed.
- Built with JUCE 8.0.4, downloaded automatically during the cloud build.
- Settings and band positions are saved inside your FL Studio project.
- Source code is in the `Source` folder; the build recipe is `CMakeLists.txt` and `.github/workflows/build.yml`.
- JUCE is licensed under AGPLv3 / commercial. Fine for personal use; check the JUCE license before distributing the plugin publicly.
