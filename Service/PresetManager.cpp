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

        const auto& allPresets = getAllPresets();
        if (allPresets.isEmpty()) {
            DBG("No presets found. Could not load preset.");
            return;
        }
        // Do nothing if builtin preset selected
        const auto currentIndex = allPresets.indexOf(presetName);
        if (currentIndex + 1 <= getNumFactoryPresets()) {
            return;
        }

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

        const auto& allPresets = getAllPresets();
        if (allPresets.isEmpty()) {
            DBG("No presets found. Could not load preset.");
            return;
        }
        const auto currentIndex = allPresets.indexOf(presetName);

        if (currentIndex + 1 <= getNumFactoryPresets()) {
            // Read builtin presets from binary data
            auto xmlSize = 0;
            juce::String binaryPresetName = presetName;
            binaryPresetName.append("_preset", sizeof(uint64_t));

            const auto presetFile = BinaryData::getNamedResource(binaryPresetName.toUTF8(), xmlSize);
            if (!presetFile) {
                DBG("preset not found");
            }

            XmlDocument xmlDocument { presetFile };

            const auto valueTreeToLoad = ValueTree::fromXml(*xmlDocument.getDocumentElement());

            valueTreeState.replaceState(valueTreeToLoad);
            currentPreset.setValue(presetName);
        } else {
            // Read user presets from files on user system
            const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
            if (!presetFile.exists()) {
                DBG("Could not find preset file: " + presetFile.getFullPathName());
                jassertfalse;
                return;
            }
            XmlDocument xmlDocument { presetFile };
            const auto valueTreeToLoad = ValueTree::fromXml(*xmlDocument.getDocumentElement());

            valueTreeState.replaceState(valueTreeToLoad);
            currentPreset.setValue(presetName);
        }
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

    // Return a string array of user preset names from XML files
    StringArray PresetManager::getUserPresets() const {
        StringArray presets;
        const auto fileArray = defaultDirectory.findChildFiles(
            File::TypesOfFileToFind::findFiles, false, "*." + extension);
        for (const auto& file : fileArray) {
            presets.add(file.getFileNameWithoutExtension());
        }
        return presets;
    }

    // Return a string array of built in preset names from binary data
    StringArray PresetManager::getFactoryPresets() const {
        StringArray presets;
        // Needs to match names of presets in Resources/FactoryPresets (without .preset suffix)
        auto factory = {"Belltolls", "Clean", "EightiesLead", "FlyingWhales", "Thrash", "UnderTheBridge"};
        for (const auto& preset : factory) {
            presets.add(preset);
        }
        return presets;
    }

    // Return a string array of all preset names
    StringArray PresetManager::getAllPresets() const {
        auto factoryPresets = getFactoryPresets();
        const auto userPresets = getUserPresets();
        factoryPresets.addArray(userPresets);
        return factoryPresets;
    }

    int PresetManager::getNumFactoryPresets() const {
        return getFactoryPresets().size();
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
