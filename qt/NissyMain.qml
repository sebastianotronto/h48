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
      property alias minmoves: solverCfg.minmoves
      property alias maxmoves: solverCfg.maxmoves
      property alias maxsolutions: solverCfg.maxsolutions
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
      SplitView.preferredHeight: 300

      component Separator: Rectangle {
        height: 1
        Layout.fillWidth: true
        color: "black"
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
        property alias maxsolutions: maxOptimal.maxsolutions

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

        RowLayout {
          id: maxOptimal

          property alias maxsolutions: maxSolsRect.maxsolutions

          Rectangle {
            width: 220
            height: 20
            Label { text: "Maximum number of solutions:" }
          }
          Rectangle {
            id: maxSolsRect

            property int maxsolutions: parseInt(textField.text)

            width: 35
            height: 20

            TextField {
              id: textField

              width: parent.width
              text: "1"
              validator: IntValidator{ bottom: 1; top: 999; }

              onAccepted: submitScramble()
            }
          }
        }
      }

      Separator {}

      Label { id: solsHeader }

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
    mainArea.solsHeader = "Loading solutions..."
    mainArea.sols = ""
    logView.text = ""
    NissyAdapter.requestSolve(
        mainArea.scramble,
        "h48h3k2",
        mainArea.minmoves,
        mainArea.maxmoves,
        mainArea.maxsolutions,
        -1
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
      mainArea.solusHeader = msg
      mainArea.sols = ""
    }
    function onAppendLog(msg) {
      logView.text += msg
    }
  }
}
