using System;
using System.Collections.Generic;

using Honeycomb.Data.Variables;
using Honeycomb.DataBinding.DataSources;
using Honeycomb.Essentials;
using Honeycomb.Essentials.Utilities;
using Honeycomb.Signals;

using UnityEngine;

namespace DP.Bonus.Pips
{
    public class ItemPips : HoneycombBehavior
    {
        #region Serialized Variables

        [SerializeField] private List<SubItemPip> subGrandPips = new(4);
        [SerializeField] private List<SubItemPip> subMajorPips = new(4);
        [SerializeField] private List<SubItemPip> subMinorPips = new(4);
        [SerializeField] private List<SubItemPip> subMiniPips = new(4);

        [SerializeField] private List<GameObject> activateOnIntro = new(3);

        [SerializeField] private GameObjectVariable targetGrandObject;

        [SerializeField] private GameObjectVariable targetMajorObject;

        [SerializeField] private GameObjectVariable targetMiniObject;

        [SerializeField] private GameObjectVariable targetMinorObject;
        
        [SerializeField] private Signal grandAwardedOn;
        [SerializeField] private Signal majorAwardedOn;
        [SerializeField] private Signal minorAwardedOn;
        [SerializeField] private Signal miniAwardedOn ;

        [SerializeField] private NumberVariable grandCounter;
        [SerializeField] private NumberVariable majorCounter;
        [SerializeField] private NumberVariable minorCounter;
        [SerializeField] private NumberVariable miniCounter;

        [SerializeField] private NumberDataSource maxGrandCounter;
        [SerializeField] private NumberDataSource maxMajorCounter;
        [SerializeField] private NumberDataSource maxMinorCounter;
        [SerializeField] private NumberDataSource maxMiniCounter;

        [SerializeField] private DecimalNumberDataSource timeToReachTarget;
        [SerializeField] private DecimalNumberDataSource pipsPreFlightDelayTime;
        [SerializeField] private DecimalNumberDataSource BannerHoldTime;

        [SerializeField] private BoolVariable haltPipsAwardTrain;

        [SerializeField] private Signal onPipsItemStarted;

        [SerializeField] private Signal onReachedTarget;

        [SerializeField] private NumberVariable pipsReachedTarget;

        #endregion

        #region Private Variables

        // Cell ID of this item placed.
        private int _cellID;

        // counters
        private long _grandCount;
        private long _majorCount;
        private long _minorCount;
        private long _miniCount;


        // Backup Initial values
        private readonly List<Vector3> _initialGrandLocalPosition = new(2);
        private readonly List<Vector3> _initialMajorLocalPosition = new(2);
        private readonly List<Vector3> _initialMinorLocalPosition = new(2);
        private readonly List<Vector3> _initialMiniLocalPosition = new(2);

        private Animator _animator;
        private static readonly int Intro = Animator.StringToHash("Intro");

        #endregion

        #region Unity Events

        protected override void PreInjection()
        {
            //TODO: write validations 
            // Validations
            Validate.Object(timeToReachTarget, nameof(timeToReachTarget));

            _animator = GetComponent<Animator>();
            if (_animator == null)
            {
                throw new MissingComponentException(nameof(_animator) + " component is missing on the " +
                                                    nameof(SubItemPip));
            }
        }

        protected override void PostInjection()
        {
            foreach (var pip in subGrandPips)
            {
                _initialGrandLocalPosition.Add(pip.gameObject.transform.localPosition);
            }

            foreach (var pip in subMajorPips)
            {
                _initialMajorLocalPosition.Add(pip.gameObject.transform.localPosition);
            }

            foreach (var pip in subMinorPips)
            {
                _initialMinorLocalPosition.Add(pip.gameObject.transform.localPosition);
            }

            foreach (var pip in subMiniPips)
            {
                _initialMiniLocalPosition.Add(pip.gameObject.transform.localPosition);
            }
        }

        #endregion

        #region Pip init, intro, show, award, hide, and reset

        public void InitPips(long grandCount, long majorCount, long minorCount, long miniCount)
        {
            gameObject.SetActive(true);

            _grandCount = grandCount;
            _majorCount = majorCount;
            _minorCount = minorCount;
            _miniCount = miniCount;
        }

        public void PlayIntro()
        {
            foreach (var go in activateOnIntro)
            {
                go.SetActive(true);
            }

            ShowPips();

            _animator.SetTrigger(Intro);
            onPipsItemStarted.Post();

            foreach (var subItemPip in subGrandPips)
            {
                subItemPip.playIntro();
            }

            foreach (var subItemPip in subMajorPips)
            {
                subItemPip.playIntro();
            }

            foreach (var subItemPip in subMinorPips)
            {
                subItemPip.playIntro();
            }

            foreach (var subItemPip in subMiniPips)
            {
                subItemPip.playIntro();
            }
        }

        public void ShowPips()
        {
            // hide all pips before showing new pips
            HideSubPips();

            for (int i = 0; i < _grandCount; i++)
            {
                subGrandPips[i].gameObject.SetActive(true);
            }

            for (int i = 0; i < _majorCount; i++)
            {
                subMajorPips[i].gameObject.SetActive(true);
            }

            for (int i = 0; i < _minorCount; i++)
            {
                subMinorPips[i].gameObject.SetActive(true);
            }

            for (int i = 0; i < _miniCount; i++)
            {
                subMiniPips[i].gameObject.SetActive(true);
            }
        }

        public void PipsAward(long pipItemCount,NumberVariable awardCounter,long maxAwardCounter,List<SubItemPip> subPips, GameObjectVariable targetObject, Signal postPipAwarded)
        {
            for (int i = (int)pipItemCount - 1; i >= 0; i--)
            {
                awardCounter.Value += 1;
                
                subPips[i].PlayFlyAnimtion(awardCounter, (int)maxAwardCounter, targetObject.Value,
                   (float)timeToReachTarget.Value, (float)pipsPreFlightDelayTime.Value, postPipAwarded);

                if (awardCounter.Value >= maxAwardCounter)
                {
                    haltPipsAwardTrain.Value = true;
                }
            }
        }

        public void PipsGrandAwarded()
        {
            PipsAward(_grandCount, grandCounter, maxGrandCounter.Value, subGrandPips, targetGrandObject, grandAwardedOn);
        }

        public void PipsMajorAwarded()
        {
            PipsAward(_majorCount, majorCounter, maxMajorCounter.Value,subMajorPips,targetMajorObject, majorAwardedOn);
        }

        public void PipsMinorAwarded()
        {
            PipsAward(_minorCount, minorCounter, maxMinorCounter.Value,subMinorPips,targetMinorObject, minorAwardedOn);
        }

        public void PipsMiniAwarded()
        {
            PipsAward(_miniCount, miniCounter, maxMiniCounter.Value, subMiniPips, targetMiniObject, miniAwardedOn);
        }

        public void HideSubPips()
        {
            foreach (var pip in subGrandPips)
            {
                pip.gameObject.SetActive(false);
            }

            foreach (var pip in subMajorPips)
            {
                pip.gameObject.SetActive(false);
            }

            foreach (var pip in subMinorPips)
            {
                pip.gameObject.SetActive(false);
            }

            foreach (var pip in subMiniPips)
            {
                pip.gameObject.SetActive(false);
            }
        }

        public void ResetSkin()
        {
            foreach (var go in activateOnIntro)
            {
                go.SetActive(false);
            }

            // Using local position for reset as it is stored at start and 
            // also calling reset on not moved object shouldn't cause any issue.

            for (var index = 0; index < _initialMinorLocalPosition.Count; index++)
            {
                var pipLocation = _initialMinorLocalPosition[index];
                subGrandPips[index].transform.localPosition = pipLocation;
            }

            for (var index = 0; index < _initialMinorLocalPosition.Count; index++)
            {
                var pipLocation = _initialMinorLocalPosition[index];
                subMajorPips[index].transform.localPosition = pipLocation;
            }

            for (var index = 0; index < _initialMinorLocalPosition.Count; index++)
            {
                var pipLocation = _initialMinorLocalPosition[index];
                subMinorPips[index].transform.localPosition = pipLocation;
            }

            for (var index = 0; index < _initialMiniLocalPosition.Count; index++)
            {
                var pipLocation = _initialMiniLocalPosition[index];
                subMiniPips[index].transform.localPosition = pipLocation;
            }

            SetLayerID(BonusConstants.PickCellLayerID);
        }

        #endregion

        #region helper methods

        public void SetCellID(int cellID)
        {
            this._cellID = cellID;
        }

        private void SetLayerID(int layerID)
        {
            foreach (var pip in subGrandPips)
            {
                pip.SetLayerID(layerID);
            }

            foreach (var pip in subMajorPips)
            {
                pip.SetLayerID(layerID);
            }

            foreach (var pip in subMinorPips)
            {
                pip.SetLayerID(layerID);
            }

            foreach (var pip in subMiniPips)
            {
                pip.SetLayerID(layerID);
            }
        }

        internal void OnPipsStarted()
        {
            SetLayerID(BonusConstants.FlyingObjectLayerID);
        }

        public void OnTargetReached()
        {
            HideSubPips();

            if (pipsReachedTarget)
            {
                pipsReachedTarget.Value = _cellID;
            }

            if (onReachedTarget)
            {
                onReachedTarget.Post();
            }

            gameObject.SetActive(false);
        }

        #endregion
    }   
}