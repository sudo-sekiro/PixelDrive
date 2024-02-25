#pragma once

#include <JuceHeader.h>

#include <memory>

#define BUTTON_COLOUR_HEX 0xFF525174

namespace UserInterface {
// UI component for choosing and creating user presets
class PresetPanel : public Component, Button::Listener, ComboBox::Listener {
 public:
    explicit PresetPanel(Service::PresetManager& pm) : presetManager(pm) {
        configureButton(saveButton, "Save");
        configureButton(deleteButton, "Delete");
        configureButton(previousPresetButton, "<");
        configureButton(nextPresetButton, ">");

        presetList.setTextWhenNothingSelected("No Preset Selected");
        presetList.setMouseCursor(MouseCursor::PointingHandCursor);
        presetList.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(BUTTON_COLOUR_HEX));
        addAndMakeVisible(presetList);
        presetList.addListener(this);
        loadPresetList();
    }

    ~PresetPanel() {
        saveButton.removeListener(this);
        deleteButton.removeListener(this);
        previousPresetButton.removeListener(this);
        nextPresetButton.removeListener(this);
        presetList.removeListener(this);
    }

    // Set component bounds when panel is resized.
    void resized() override {
        const auto container = getLocalBounds().reduced(4);
        auto bounds = container;

        saveButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2f)).reduced(4));
        previousPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
        presetList.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.4f)).reduced(4));
        nextPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
        deleteButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2f)).reduced(4));
    }

 private:
    // Call back method for all buttons within the panel
    void buttonClicked(Button* button) override {
        if (button == &saveButton) {
            fileChooser = std::make_unique<FileChooser> (
                "Please enter the preset name to save",
                Service::PresetManager::defaultDirectory,
                "*." + Service::PresetManager::extension);
            fileChooser->launchAsync(FileBrowserComponent::saveMode,
                                    [&](const FileChooser& chooser) {
                                        const auto resultFile = chooser.getResult();
                                        presetManager.savePreset(resultFile.getFileNameWithoutExtension());
                                        loadPresetList();
                                    });
        } else if (button == &previousPresetButton) {
            const auto index = presetManager.loadPreviousPreset();
            presetList.setSelectedItemIndex(index, dontSendNotification);
        } else if (button == &nextPresetButton) {
            const auto index = presetManager.loadNextPreset();
            presetList.setSelectedItemIndex(index, dontSendNotification);
        } else if (button == &deleteButton) {
            // Todo add dialogue confirmation button
            presetManager.deletePreset(presetManager.getCurrentPreset());
            loadPresetList();
        }
    }

    // If a preset is selected in using the combo box, load that preset
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override {
        if (comboBoxThatHasChanged == &presetList) {
            presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
        }
    }

    // Called for each button to configure behaviour
    void configureButton(Button& button, const String& buttonText) {
        button.setButtonText(buttonText);
        button.setMouseCursor(MouseCursor::PointingHandCursor);
        addAndMakeVisible(button);
        button.addListener(this);
        button.setColour(juce::TextButton::buttonColourId, juce::Colour(BUTTON_COLOUR_HEX));
        button.setConnectedEdges(juce::TextButton::ConnectedOnLeft | juce::TextButton::ConnectedOnRight);
    }

    // Update combo box options with most recent set of presets
    void loadPresetList() {
        presetList.clear(dontSendNotification);
        const auto allPresets = presetManager.getAllPresets();
        const auto factoryPresets = presetManager.getFactoryPresets();
        const auto userPresets = presetManager.getUserPresets();
        const auto currentPreset = presetManager.getCurrentPreset();

        int itemID = 1;

        PopupMenu factoryPresetMenu;
        for (auto item : factoryPresets) {
            factoryPresetMenu.addItem(itemID++, item, true, false);
        }
        presetList.getRootMenu()->addSubMenu("Factory Presets", factoryPresetMenu);

        PopupMenu userPresetMenu;
        for (auto item : userPresets) {
            userPresetMenu.addItem(itemID++, item, true, false);
        }
        presetList.getRootMenu()->addSubMenu("User Presets", userPresetMenu);

        presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), dontSendNotification);
    }

    Service::PresetManager presetManager;
    TextButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
    ComboBox presetList;
    std::unique_ptr<FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel);
};
}  // namespace UserInterface
