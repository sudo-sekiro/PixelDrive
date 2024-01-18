#include "PresetManager.h"

#include "JuceHeader.h"

namespace Service {
    const File PresetManager::defaultDirectory {
        File::getSpecialLocation(
            File::SpecialLocationType::commonDocumentsDirectory)
            .getChildFile(ProjectInfo::companyName)
            .getChildFile(ProjectInfo::projectName)
    };
    const String PresetManager::extension{ "preset" };
    const String PresetManager::presetNameProperty{ "presetName" };

    PresetManager::PresetManager(AudioProcessorValueTreeState& apvts) :
        valueTreeState(apvts) {
        // Create a default directory to store plugins
        if (!defaultDirectory.exists()) {
            const auto result = defaultDirectory.createDirectory();
            if (result.failed()) {
                DBG("Could not create preset directory: " +
                    result.getErrorMessage());
                jassertfalse;
            }
        }
        valueTreeState.state.addListener(this);
        currentPreset.referTo(valueTreeState.state.getPropertyAsValue(presetNameProperty, nullptr));
    }

    PresetManager::~PresetManager() {
        valueTreeState.state.removeListener(this);
    }

    // Save current parameters in the value tree state to a text file
    void PresetManager::savePreset(const String& presetName) {
        if (presetName.isEmpty())
            return;

        currentPreset.setValue(presetName);
        valueTreeState.state.setProperty(presetNameProperty, currentPreset, nullptr);
        const auto xml = valueTreeState.copyState().createXml();
        const auto presetFile = defaultDirectory.getChildFile(presetName
                                    + "." + extension);
        if (!xml->writeTo(presetFile)) {
            DBG("Could not create presetFile: " + presetFile.getFullPathName());
            jassertfalse;
        }
    }

    // Delete a given preset text file
    void PresetManager::deletePreset(const String& presetName) {
        if (presetName.isEmpty())
            return;

        const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
        if (!presetFile.exists()) {
            DBG("Could not find preset file: " + presetFile.getFullPathName());
            jassertfalse;
            return;
        }
        if (!presetFile.deleteFile()) {
            DBG("Could not delete preset file: " + presetFile.getFullPathName());
            jassertfalse;
            return;
        }
        currentPreset.setValue("");
    }

    // Load a value tree state from the relevant text file of a given preset
    void PresetManager::loadPreset(const String& presetName) {
        if (presetName.isEmpty())
            return;

        const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
        if (!presetFile.exists()) {
            DBG("Could not find preset file: " + presetFile.getFullPathName());
            jassertfalse;
            return;
        }
        // presetFile (XML) -> (ValueTree)
        XmlDocument xmlDocument { presetFile };
        const auto valueTreeToLoad = ValueTree::fromXml(*xmlDocument.getDocumentElement());

        valueTreeState.replaceState(valueTreeToLoad);
        currentPreset.setValue(presetName);
    }

    // Load the next preset in the preset list. Loop around if necessary.
    int PresetManager::loadNextPreset() {
        const auto& allPresets = getAllPresets();
        if (allPresets.isEmpty())
            return -1;
        const auto currentIndex = allPresets.indexOf(getCurrentPreset());
        const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
        loadPreset(allPresets.getReference(nextIndex));
        return nextIndex;
    }

    // Load the previous preset in the preset list. Loop around if necessary.
    int PresetManager::loadPreviousPreset() {
        const auto& allPresets = getAllPresets();
        if (allPresets.isEmpty())
            return -1;
        const auto currentIndex = allPresets.indexOf(getCurrentPreset());
        const auto previousIndex = currentIndex - 1 >= 0 ? currentIndex - 1 : allPresets.size() - 1;
        loadPreset(allPresets.getReference(previousIndex));
        return previousIndex;
    }

    // Return a string array of all preset names
    StringArray PresetManager::getAllPresets() const {
        StringArray presets;
        const auto fileArray = defaultDirectory.findChildFiles(
            File::TypesOfFileToFind::findFiles, false, "*." + extension);
        for (const auto& file : fileArray) {
            presets.add(file.getFileNameWithoutExtension());
        }
        return presets;
    }

    // Return the current preset name
    String PresetManager::getCurrentPreset() const {
        return currentPreset.toString();
    }

    // Set the refer the preset name property to the current preset value whenever the value tree is overwritten.
    void PresetManager::valueTreeRedirected(ValueTree& treeWhichHasBeenChanged) {
        currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetNameProperty, nullptr));
    }
}  // namespace Service
