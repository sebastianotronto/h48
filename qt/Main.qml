import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
  id: mainWindow

  width: 800
  height: 600
  visible: true
  title: "Nissy 3.0 - Preview"

  SplitView {
    id: splitView
    anchors.fill: parent
    orientation: Qt.Vertical

    handle: Rectangle {
      implicitHeight: 3
      color: SplitHandle.hovered ? "black" : "#AAAAAA"
    }

    component MyScrollBar: ScrollBar {
      orientation: Qt.Vertical
      size: parent.height
      policy: ScrollBar.AlwaysOn
      anchors.right: parent.right
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      contentItem: Rectangle {
        implicitWidth: 4
        radius: implicitWidth/2
        color: "black"
      }
      background: Rectangle {
        implicitWidth: 4
        radius: implicitWidth/2
        color: "#AAAAAA"
      }
    }

    ColumnLayout {
      id: mainArea

      property alias scramble: scrambleRow.scramble
      property alias solver: solverCfg.solver
      property alias minmoves: solverCfg.minmoves
      property alias maxmoves: solverCfg.maxmoves
      property alias maxsolutions: solverCfg.maxsolutions
      property alias optimal: solverCfg.optimal
      property alias sols: sols.text
      property alias solsHeader: solsHeader.text

      property bool solutionsLoading: false

      anchors.top: parent.top
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.bottom: logView.top
      anchors.margins: 6
      spacing: 10

      SplitView.minimumHeight: 180
      SplitView.preferredHeight: 500

      component Separator: Rectangle {
        height: 1
        Layout.fillWidth: true
        color: "black"
      }

      component OptionalValue: RowLayout {
        property alias currentValue: valueRect.value
        property alias from: spinBox.from
        property alias to: spinBox.to
        property alias defaultValue: spinBox.value
        property alias defaultEnabled: sw.checked
        property alias label: sw.text
        property int defaultSavedValue: 1
        property int savedValue: defaultSavedValue

        Switch {
          id: sw

          checked: true

          onToggled: () => {
            if (checked) {
              currentValue = savedValue
            } else {
              savedValue = currentValue
              currentValue = spinBox.to
            }
          }
        }

        Rectangle {
          id: valueRect

          property alias enabled: sw.checked
          property alias value: spinBox.value

          width: 65
          height: 20

          SpinBox {
            id: spinBox

            width: parent.width
            editable: true
            enabled: parent.enabled
          }
        }
      }

      ColumnLayout {
        id: scrambleRow

        property alias scramble: scrambleRowLayout.scramble

        RowLayout {
          id: scrambleRowLayout

          property alias scramble: scrambleEditor.text

          spacing: 6

          TextField {
            id: scrambleEditor

            placeholderText: "Enter scramble here"
            Layout.fillWidth: true
            padding: 4

            readonly property bool empty: text.trim().length == 0
            readonly property bool valid: NissyAdapter.isValidScramble(text)

            onAccepted: if (!empty && valid) submitScramble()
          }

          Button {
            id: solveButton

            enabled: !scrambleEditor.empty && scrambleEditor.valid &&
              !mainArea.solutionsLoading
            text: "Solve!"

            onPressed: submitScramble()
          }
        }

        Label {
          id: invalidScrambleWarning
          text: scrambleEditor.empty || scrambleEditor.valid ?
            "" : "Invalid Scramble"
        }
      }

      Separator {}

      ColumnLayout {
        id: solverCfg

        property alias minmoves: minMaxRow.min
        property alias maxmoves: minMaxRow.max
        property alias maxsolutions: maxSols.currentValue
        property alias optimal: optimal.currentValue
        property alias solver: solverRow.solver

        RowLayout {
          id: solverRow

          property alias solver: comboBox.currentValue

          Label { text: "Solver" }
          ComboBox {
            id: comboBox

            currentIndex: 3
            textRole: "text"
            valueRole: "name"
            implicitContentWidthPolicy: ComboBox.WidestTextWhenCompleted

            model: ListModel {
              ListElement { text: "h48 h=0, k=4 (59 Mb)"; name: "h48h0k4" }
              ListElement { text: "h48 h=1, k=2 (115 Mb)"; name: "h48h1k2" }
              ListElement { text: "h48 h=2, k=2 (171 Mb)"; name: "h48h2k2" }
              ListElement { text: "h48 h=3, k=2 (283 Mb)"; name: "h48h3k2" }
              ListElement { text: "h48 h=7, k=2 (3.6 Gb)"; name: "h48h7k2" }
            }
          }
        }

        RowLayout {
          id: minMaxRow

          property alias min: slider.min
          property alias max: slider.max

          Rectangle {
            width: 100
            height: 20
            Label { text: "Min moves: " + slider.min }
          }
          RangeSlider {
            id: slider
            from: 0
            to: 20
            first.value: from
            second.value: to
            stepSize: 1
            snapMode: RangeSlider.SnapAlways

            readonly property int min: Math.round(first.value)
            readonly property int max: Math.round(second.value)
          }
          Rectangle {
            width: 100
            height: 20
            Label { text: "Max moves: " + slider.max }
          }
        }

        OptionalValue {
          id: optimal

          label: "Above optimal by at most"
          from: 0
          to: 20
          defaultValue: 20
          defaultEnabled: false
          defaultSavedValue: 0
        }

        OptionalValue {
          id: maxSols

          label: "Limit number of solutions to"
          from: 1
          to: 999
          defaultValue: 1
          defaultEnabled: true
          defaultSavedValue: 1
        }
      }

      Separator {}

      StackLayout {
        Layout.maximumHeight: 30
        currentIndex: mainArea.solutionsLoading ? 0 : 1

        BusyIndicator { running: mainArea.solutionsLoading }
        Label { id: solsHeader }
      }

      ScrollView {
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.bottomMargin: 10
        ScrollBar.vertical: MyScrollBar {}
        
        TextEdit {
          id: sols
          readOnly: true
          font.family: "Monospace"
        }
      }
    }

    ScrollView {
      id: logView

      property alias text: logText.text

      anchors.left: parent.left
      anchors.right: parent.right
      anchors.bottom: parent.bottom
      anchors.margins: 6

      SplitView.preferredHeight: 300

      background: Rectangle {
        color: "#404040"
        radius: 4
      }

      ScrollBar.vertical: MyScrollBar {
        id: scrollBar
        position: 1.0 - size
      }
      Label {
        id: logText

        font.family: "Monospace"
        color: "white"
      }
    }
  }

  function submitScramble() {
    mainArea.solutionsLoading = true;
    mainArea.solsHeader = ""
    mainArea.sols = ""
    logView.text = ""
    NissyAdapter.requestSolve(
        mainArea.scramble,
        mainArea.solver,
        mainArea.minmoves,
        mainArea.maxmoves,
        mainArea.maxsolutions,
        mainArea.optimal
    )
  }

  Connections {
    target: NissyAdapter
    function onSolutionsReady(header, sols) {
      mainArea.solutionsLoading = false
      mainArea.solsHeader = header
      mainArea.sols = sols
    }
    function onSolverError(msg) {
      mainArea.solutionsLoading = false
      mainArea.solsHeader = msg
      mainArea.sols = ""
    }
    function onAppendLog(msg) {
      logView.text += msg
    }
  }
}
